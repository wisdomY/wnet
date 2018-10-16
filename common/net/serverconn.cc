#include "evpp/logging.h"
#include "serverconn.h"
#include "controlpacket.h"

ServerConnection::ServerConnection(evpp::TCPConn* conn, IPacketStreamer* streamer, IServerAdapter* adapter)
    : conn_(conn), streamer_(streamer), adapter_(adapter), gotHeader_(false)
{
    if (conn == NULL || streamer == NULL || adapter == NULL) {
        LOG_ERROR << "ServerConnection(): invalid arguments";
    }
}

ServerConnection::~ServerConnection()
{
}

evpp::TCPConn* ServerConnection::GetTCPConn()
{
    return conn_;
}

bool ServerConnection::IsConnected()
{
    return conn_ != NULL ? conn_->IsConnected() : false;
}

bool ServerConnection::PostPacket(Packet *packet)
{
    if (!IsConnected()) {
        LOG_WARN << "Post packet to disconnected connection";
        return false;
    }
    if (streamer_->ExistPacketHeader()) {
        uint32_t chid = packet->GetChannelId();
        assert(chid != 0);
    }
    mutex_.lock();
    streamer_->Encode(packet, &output_);
    packet->Free();
    if (output_.length() > 0) {
        conn_->Send(&output_);
    }
    mutex_.unlock();
    return true;
}

void ServerConnection::OnMessage(const evpp::TCPConnPtr& /*conn*/, evpp::Buffer* input)
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

bool ServerConnection::HandlePacket(evpp::Buffer *input, PacketHeader *header)
{
    Packet *packet;

    packet = streamer_->Decode(input, header);
    if (packet == NULL) {
        LOG_WARN << "Unable to decode packet";
        packet = &ControlPacket::BadPacket;
    }
    else {
        packet->SetPacketHeader(header);
        if (adapter_->BatchPushPacket()) {
            inputQueue_.Push(packet);
            if (inputQueue_.Size() >= 15) {
                adapter_->HandleBatchPacket(this, inputQueue_);
                inputQueue_.Clear();
            }
            return true;
        }
    }

    adapter_->HandlePacket(this, packet);
    return true;
}

std::string ServerConnection::GetPeerIP()
{
    if (conn_ == nullptr) {
        return "";
    }
    else {
        std::string address = conn_->remote_addr();
        size_t index = address.find_first_of(':');
        if (index == std::string::npos) {
            return "";
        }
        std::string ip = address.substr(0, index);
        return ip;
    }
}


