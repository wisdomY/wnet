#pragma once

#include "evpp/buffer.h"

class Packet;

class ControlPacket : public Packet {
public:
    enum {
        CMD_BAD_PACKET = 1,
        CMD_TIMEOUT_PACKET,
        CMD_DISCONN_PACKET
    };

    static ControlPacket BadPacket;
    static ControlPacket TimeoutPacket;
    static ControlPacket DisconnPacket;

public:
    ControlPacket(int c) : command_(c)
    {
    }

    bool IsRegularPacket()
    {
        return false;
    }

    void Free()
    {
    }

    void CountDataLen()
    {
    }

    bool Encode(evpp::Buffer*)
    {
        return false;
    }

    bool Decode(evpp::Buffer*, PacketHeader*)
    {
        return false;
    }

    int GetCommand()
    {
        return command_;
    }

private:
    int command_;
};


