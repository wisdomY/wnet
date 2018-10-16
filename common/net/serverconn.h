#pragma once

#include <memory>
#include <mutex>
#include "evpp/tcp_conn.h"
#include "packet.h"
#include "ipacketfactory.h"
#include "ipacketstreamer.h"
#include "iserveradapter.h"

class ServerConnection : public std::enable_shared_from_this<ServerConnection>
{
public:
    ServerConnection(evpp::TCPConn* conn_, IPacketStreamer* streamer, IServerAdapter* adapter);

    virtual ~ServerConnection();

    bool IsConnected();

    evpp::TCPConn* GetTCPConn();

    bool PostPacket(Packet *packet);

    bool HandlePacket(evpp::Buffer *input, PacketHeader *header);

    void OnMessage(const evpp::TCPConnPtr& conn, evpp::Buffer* input);

    std::string GetPeerIP();

protected:
    evpp::TCPConn *conn_;
    IPacketStreamer *streamer_;
    IServerAdapter *adapter_;

    evpp::Buffer output_;
    PacketQueue inputQueue_;
    PacketHeader packetHeader_;
    std::mutex mutex_;
    bool gotHeader_;
};
    
typedef std::shared_ptr<ServerConnection> ServerConnectionPtr;


