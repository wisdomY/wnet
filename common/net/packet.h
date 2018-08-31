#pragma once

#include "evpp/buffer.h"

#define QYNET_PACKET_FLAG 0x51594b4a //QYKJ

class Channel;

class PacketHeader {
public:
    uint32_t chid_;
    int pcode_;
    int dataLen_;
};

class Packet {
    friend class PacketQueue;

public:
    Packet();

    virtual ~Packet();

    void SetChannelId(uint32_t chid)
    {
        packetHeader_.chid_ = chid;
    }

    uint32_t GetChannelId() const
    {
        return packetHeader_.chid_;
    }

    void SetPCode(int pcode)
    {
        packetHeader_.pcode_ = pcode;
    }

    int GetPCode() const
    {
        return packetHeader_.pcode_;
    }

    PacketHeader* GetPacketHeader()
    {
        return &packetHeader_;
    }

    void SetPacketHeader(PacketHeader *header)
    {
        if (header) {
            memcpy(&packetHeader_, header, sizeof(PacketHeader));
        }
    }

    virtual void Free()
    {
        delete this;
    }

    virtual bool IsRegularPacket()
    {
        return true;
    }

    virtual bool Encode(evpp::Buffer *output) = 0;

    virtual bool Decode(evpp::Buffer *input, PacketHeader *header) = 0;

    int64_t GetExpireTime() const
    {
        return expireTime_;
    }

    void SetExpireTime(int milliseconds);

    void SetChannel(Channel *channel);

    Channel* GetChannel() const
    {
        return channel_;
    }

    Packet* GetNext() const
    {
        return next_;
    }

protected:
    PacketHeader packetHeader_;
    int64_t expireTime_;
    Channel *channel_;
    Packet *next_;
};
