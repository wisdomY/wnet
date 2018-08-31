/*********************************************************************************************
* �����У���tcp��Ϣ�����ŵ���������д���
*********************************************************************************************/
#pragma once

#include "packet.h"

class PacketQueue
{
public:
    PacketQueue();

    ~PacketQueue();

    Packet *Pop();

    void Clear();

    void Push(Packet *packet);

    int Size();

    bool Empty();

    void MoveTo(PacketQueue *destQueue);

    Packet *GetTimeoutList(int64_t now);

    Packet *GetPacketList();

    Packet *Head()
    {
        return head_;
    }

    Packet* Tail()
    {
        return tail_;
    }
protected:
    Packet *head_;
    Packet *tail_;
    int size_;
};

