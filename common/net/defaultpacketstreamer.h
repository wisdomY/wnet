#pragma once

#include "evpp/buffer.h"

#include "packet.h"
#include "ipacketfactory.h"
#include "ipacketstreamer.h"

class DefaultPacketStreamer : public IPacketStreamer
{
public:
    DefaultPacketStreamer();

    DefaultPacketStreamer(IPacketFactory *factory);

    ~DefaultPacketStreamer();

    void SetPacketFactory(IPacketFactory *factory);

    bool GetPacketInfo(evpp::Buffer *input, PacketHeader *header, bool *broken);

    Packet *Decode(evpp::Buffer *input, PacketHeader *header);

    bool Encode(Packet *packet, evpp::Buffer *output);

    static void SetPacketFlag(int flag);

public:
    static int nPacketFlag_;
};

