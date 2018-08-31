#pragma once

#include "evpp/tcp_conn.h"
#include "packet.h"
#include "packetqueue.h"
#include "ipackethandler.h"

class ServerConnection;

class IServerAdapter {

public:
    virtual IPacketHandler::HPRetCode HandlePacket(ServerConnection *conn, Packet *packet) = 0;
    virtual bool HandleBatchPacket(ServerConnection*, PacketQueue&) {
        return false;
    }

    IServerAdapter() {
        batchPushPacket_ = false;
    }
    virtual ~IServerAdapter() {}

    void SetBatchPushPacket(bool value) {
        batchPushPacket_ = value;
    }

    bool BatchPushPacket() {
        return batchPushPacket_;
    }

private:
    bool batchPushPacket_;
};
