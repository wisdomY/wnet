#pragma once

class Packet;

class IPacketFactory
{
public:
    virtual ~IPacketFactory() {}
    virtual Packet* CreatePacket(int pcode) = 0;
};

