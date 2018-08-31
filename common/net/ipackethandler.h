#pragma once

class Packet;

class IPacketHandler {
public:
    enum HPRetCode {
        KEEP_CHANNEL  = 0,
        CLOSE_CHANNEL = 1,
        FREE_CHANNEL  = 2
    };

    virtual ~IPacketHandler() {}
    virtual HPRetCode HandlePacket(Packet *packet, void *args) = 0;
};
