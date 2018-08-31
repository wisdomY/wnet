#pragma once

#include "evpp/buffer.h"

class IPacketStreamer {

public:
    IPacketStreamer() {
        factory_ = NULL;
        existPacketHeader_ = true;
    }

    IPacketStreamer(IPacketFactory* factory) {
        factory_ = factory;
        existPacketHeader_ = true;
    }

    virtual ~IPacketStreamer() {};

    virtual bool GetPacketInfo(evpp::Buffer *input, PacketHeader *header, bool *broken) = 0;

    virtual Packet* Decode(evpp::Buffer *input, PacketHeader *header) = 0;

    virtual bool Encode(Packet *packet, evpp::Buffer *output) = 0;

    bool ExistPacketHeader() {
        return existPacketHeader_;
    }

protected:
    IPacketFactory *factory_;
    bool existPacketHeader_;
};
