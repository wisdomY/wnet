#pragma once

#include <mutex>
#include <condition_variable>

#include "evpp/tcp_conn.h"
#include "evpp/tcp_server.h"
#include "packet.h"
#include "ipacketfactory.h"
#include "ipacketstreamer.h"
#include <list>
class IServerAdapter;

class ServerBase {

public:
    ServerBase(evpp::EventLoop* loop,
               IPacketStreamer* streamer,
               IServerAdapter* adapter,
               const std::string& listen_addr/*ip:port*/,
               const std::string& name,
               uint32_t thread_num);

    virtual ~ServerBase();

    virtual void OnConnection(const evpp::TCPConnPtr& conn);

    void Start();

    void Stop();

protected:
    void ConnectionCallback(const evpp::TCPConnPtr& conn);

    void OnMessage(const evpp::TCPConnPtr& conn, evpp::Buffer* buf);

protected:
    evpp::EventLoop *loop_;
    IPacketStreamer *streamer_;
    IServerAdapter *adapter_;
    evpp::TCPServer tcp_server_;
private:
    bool stopped_;
    std::mutex stopMutex_;
    std::condition_variable stopCond_;
};
    
