#include <chrono>

#include "evpp/logging.h"

#include "channel.h"
#include "clientsession.h"
#include "controlpacket.h"

ClientSession::ClientSession(evpp::EventLoop *l, IPacketStreamer *s,
    const std::string& raddr, const std::string& n)
    : loop_(l), tcp_client_(l, raddr, n), streamer_(s), gotHeader_(false)
{
    defaultPacketHandler_ = NULL;
    queueTimeout_ = 5000;
    queueLimit_ = 50;
    queueTotalSize_ = 0;
    client_conn_cb_ = DefaultClientConnectionCallback;
    tcp_client_.SetConnectionCallback(std::bind(&ClientSession::OnConnection, this, std::placeholders::_1));
    tcp_client_.SetMessageCallback(std::bind(&ClientSession::OnMessage, this, std::placeholders::_1, std::placeholders::_2));
    memset(&packetHeader_, 0, sizeof(packetHeader_));
}

ClientSession::~ClientSession()
{
}

void ClientSession::Disconnect()
{
    if (IsConnected()) {
        tcp_client_.Disconnect();
        do {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } while (IsConnected());
    }
    CheckTimeout(QYNET_MAX_TIME);
}

void ClientSession::Stop()
{
    if (loop_ != NULL) {
        loop_->Stop();
    }
}

void ClientSession::Connect(const ClientConnectionCallback& cb) {
    client_conn_cb_ = cb;
    tcp_client_.Connect();
}

void ClientSession::SetAutoReconnect(evpp::Duration duration)
{
    tcp_client_.set_reconnect_interval(duration);
    tcp_client_.set_auto_reconnect(true);
}


void ClientSession::OnConnection(const evpp::TCPConnPtr& conn) {
    if (conn->IsConnected()) {
        DLOG_TRACE << "Tcp client session setup fd:" << conn->fd() << " id:" << conn->id();
        conn->SetTCPNoDelay(true);
        connPtr_ = conn;
        PostOutputQueue();
        client_conn_cb_(this);
    }
    else {
        DLOG_TRACE << "Tcp client session destroyed id:" << conn->id();
        client_conn_cb_(this);//断连也需要调用回调，回调到业务层处理
    }
}


/************************************************************************************************************
* 收到数据的响应回调，有效的数据会调用HandlePacket来处理
************************************************************************************************************/
void ClientSession::OnMessage(const evpp::TCPConnPtr& /*conn*/, evpp::Buffer* input)
{
    bool broken = false;

    while (input->length() > 0) {
        if (!gotHeader_) {
            gotHeader_ = streamer_->GetPacketInfo(input, &packetHeader_, &broken);
            if (broken) {
                break;
            }
        }
        if (gotHeader_ && static_cast<int>(input->length()) >= packetHeader_.dataLen_) {
            HandlePacket(input, &packetHeader_);
            gotHeader_ = false;
            packetHeader_.dataLen_ = 0;
        }
        else {
            break;
        }
    }

    if (broken) {
        gotHeader_ = false;
    }
}

bool ClientSession::PostOutputQueue()
{
    if (!IsConnected()) {
        return false;
    }
    std::lock_guard<std::mutex> _guard(mutex_);
    if (outputQueue_.Size() > 0) {
        while (outputQueue_.Size() > 0) {
            Packet *packet = outputQueue_.Pop();
            streamer_->Encode(packet, &output_);
            channelPool_.SetExpireTime(packet->GetChannel(), packet->GetExpireTime());
            packet->Free();
            if (output_.length() > 0) {
                connPtr_->Send(&output_);
            }
        }
        if (queueLimit_ > 0 && queueTotalSize_ > queueLimit_) {
            queueTotalSize_ = outputQueue_.Size() + channelPool_.GetUseListCount();
            if (queueTotalSize_ <= queueLimit_) {
                outputCond_.notify_one();
            }
        }
    }
    return true;
}


/************************************************************************************************************
* 发送数据包函数，默认是非阻塞的
* 若连接不存在，则判断是否自动重新建立连接，若是自动建立连接，丢失的包最多保存十个
* 根据是否存在包头，判断是否创建会话(channel这里我理解成会话)
* 未连接的时候且队列中包数不超过10个的时候将包加到队列中
************************************************************************************************************/
bool ClientSession::PostPacket(Packet *packet, IPacketHandler *packetHandler, void *args, bool noblocking)
{
    if (!IsConnected()) {
        if (tcp_client_.auto_reconnect() == false) {
            return false;
        }
        else if (outputQueue_.Size() > 10) {
            return false;
        }
    }

    {
        std::lock_guard<std::mutex> _guard(mutex_);
        queueTotalSize_ = outputQueue_.Size() + channelPool_.GetUseListCount();
        if (queueLimit_ > 0 && noblocking && queueTotalSize_ >= queueLimit_) {
            return false;
        }
    }

    Channel *channel = NULL;
    packet->SetExpireTime(queueTimeout_);
    if (streamer_->ExistPacketHeader()) {
        channel = channelPool_.AllocChannel();
        if (channel == NULL) {
            LOG_WARN << "allocate channel failed";
            return false;
        }

        channel->SetHandler(packetHandler);
        channel->SetArgs(args);
        packet->SetChannel(channel);
    }
    else {
        packet->SetChannel(NULL);
    }

    mutex_.lock();
    if (!IsConnected()) {//TCP未连接的时候才将消息放到队列中，重连的时候保证消息不丢失，已连接的时候会直接发送出去
        outputQueue_.Push(packet);
    }
    else {
        streamer_->Encode(packet, &output_);
        channelPool_.SetExpireTime(packet->GetChannel(), packet->GetExpireTime());
        packet->Free();
        if (output_.length() > 0) {
            connPtr_->Send(&output_);
        }
    }
    mutex_.unlock();

//TODO 阻塞的情况需要理解
    if (queueLimit_ > 0 && noblocking == false) {
        std::unique_lock<std::mutex> _lk(mutex_);
        while (outputCond_.wait_until(_lk, std::chrono::system_clock::now() + std::chrono::milliseconds(1000), [this]() {
            queueTotalSize_ = outputQueue_.Size() + channelPool_.GetUseListCount();
            bool stopped = loop_->IsStopped();
            bool connected = IsConnected();
            return queueTotalSize_ <= queueLimit_ || stopped || !connected;
            }) == false);
    }
            
    return true;
}



/************************************************************************************************************
* 收到数据的响应时由回调函数调用，根据这个报文类型(是否包含包头决定如何处理: 存在包头即存在会话)
* 调用报文的解包函数解包，(channel会话中会保存参数、处理句柄的),然后再调用处理函数处理这个包的信息
* 最后调用处理句柄的HandlePacket方法处理包信息
************************************************************************************************************/
bool ClientSession::HandlePacket(evpp::Buffer *input, PacketHeader *header)
{
    Packet *packet;
    IPacketHandler::HPRetCode rc;
    void *args = NULL;
    Channel *channel = NULL;
    IPacketHandler *packetHandler = NULL;

    if (streamer_->ExistPacketHeader()) {
        uint32_t chid = header->chid_;
        chid = (chid & 0xFFFFFFF);
        channel = channelPool_.OfferChannel(chid);

        if (channel == NULL) {
            input->Skip(header->dataLen_);
            LOG_WARN << "Not found channel: " << chid;
            return false;
        }

        packetHandler = channel->GetHandler();
        args = channel->GetArgs();
    }

    packet = streamer_->Decode(input, header);
    if (packet == NULL) {
        LOG_WARN << "Unable to decode packet";
        packet = &ControlPacket::BadPacket;
    }
    else {
        packet->SetPacketHeader(header);
    }

    if (packetHandler == NULL) {
        packetHandler = defaultPacketHandler_;
    }
    assert(packetHandler != NULL);

    rc = packetHandler->HandlePacket(packet, args);
    if (channel) {
        channel->SetArgs(NULL);
        channelPool_.AppendChannel(channel);
    }
    return true;
}

//检查超时消息所占用的channel，需要超时channel清理
bool ClientSession::CheckTimeout(int64_t now)
{
    Channel *list = channelPool_.GetTimeoutList(now);
    Channel *channel = NULL;
    IPacketHandler *packetHandler = NULL;

//超时的channel处理
    if (list != NULL) {
        channel = list;
        while (channel != NULL) {
            packetHandler = channel->GetHandler();
            if (packetHandler == NULL) {
                packetHandler = defaultPacketHandler_;
            }
            if (packetHandler != NULL) {
                packetHandler->HandlePacket(&ControlPacket::TimeoutPacket, channel->GetArgs());
                channel->SetArgs(NULL);
            }
            channel = channel->GetNext();
        }
       
        channelPool_.AppendFreeList(list);
    }

//已经返回的消息队列里面的超时消息处理
    mutex_.lock();
    Packet *packetList = outputQueue_.GetTimeoutList(now);
    mutex_.unlock();
    while (packetList) {
        Packet *packet = packetList;
        packetList = packetList->GetNext();
        channel = packet->GetChannel();
        packet->Free();
        if (channel) {
            packetHandler = channel->GetHandler();
            if (packetHandler == NULL) {
                packetHandler = defaultPacketHandler_;
            }
            if (packetHandler != NULL) {
                packetHandler->HandlePacket(&ControlPacket::TimeoutPacket, channel->GetArgs());
                channel->SetArgs(NULL);
            }
            channelPool_.FreeChannel(channel);
        }
    }

    if (queueLimit_ > 0 && queueTotalSize_ > queueLimit_) {
        std::unique_lock<std::mutex> _lk(mutex_);
        queueTotalSize_ = outputQueue_.Size() + channelPool_.GetUseListCount();
        if (queueTotalSize_ <= queueLimit_) {
            outputCond_.notify_one();
        }
    }
    return true;
}


