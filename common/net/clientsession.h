#pragma once

#include <string>
#include <condition_variable>

#include "evpp/tcp_client.h"
#include "evpp/tcp_conn.h"

#include "packet.h"
#include "packetqueue.h"
#include "ipackethandler.h"
#include "ipacketfactory.h"
#include "ipacketstreamer.h"
#include "channelpool.h"

class ClientSession;

typedef std::function<void(ClientSession*)> ClientConnectionCallback;
inline void DefaultClientConnectionCallback(ClientSession*) {}

class ClientSession
{
public:
    ClientSession(evpp::EventLoop* loop,
                  IPacketStreamer* streamer,
                  const std::string& remote_addr/*host:port*/,
                  const std::string& name);
    virtual ~ClientSession();

    void SetDefaultPacketHandler(IPacketHandler* ph)
    {
        defaultPacketHandler_ = ph;
    }

    virtual bool PostPacket(Packet* packet, IPacketHandler* packetHandler = NULL, void* args = NULL, bool noblocking = true);

    virtual bool HandlePacket(evpp::Buffer *input, PacketHeader *header);

    void Connect(const ClientConnectionCallback& cb);
    void SetAutoReconnect(evpp::Duration duration);
    void Disconnect();
    void Stop();

    bool IsConnected()
    {
        return connPtr_ != NULL ? connPtr_->IsConnected() : false;
    }

    bool CheckTimeout(int64_t now);

    void SetQueueLimit(int limit)
    {
        queueLimit_ = limit;
    }

private:
    bool PostOutputQueue();

    void OnConnection(const evpp::TCPConnPtr& conn);

    void OnMessage(const evpp::TCPConnPtr& conn, evpp::Buffer* buf);

protected:
    evpp::EventLoop *loop_;
    evpp::TCPClient tcp_client_;
    evpp::TCPConnPtr connPtr_;
    IPacketHandler *defaultPacketHandler_;
    IPacketStreamer *streamer_;

    evpp::Buffer output_;
    PacketQueue outputQueue_;//包队列
    std::mutex mutex_;
    std::condition_variable outputCond_;
    ChannelPool channelPool_;//相当于会话池
    int queueTimeout_;
    int queueTotalSize_;
    int queueLimit_;
    bool gotHeader_;

private:
    PacketHeader packetHeader_;
    ClientConnectionCallback client_conn_cb_;
};
    
