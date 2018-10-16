#include "evpp/gettimeofday.h"
#include "channel.h"
#include "packet.h"

Packet::Packet() : next_(NULL), channel_(NULL), expireTime_(0)
{
    memset(&packetHeader_, 0, sizeof(PacketHeader));
}

Packet::~Packet()
{
}


void Packet::SetChannel(Channel *channel)
{
    if (channel) {
        channel_ = channel;
        packetHeader_.chid_ = channel->GetId();
    }
}


//���ð��ĳ�ʱʱ�䣬��ʱʱ�䵥λΪus(���Ǵ˷����Ĳ�����λΪms�������Ҫ*1000)����������0������ʱʱ��Ϊ24Сʱ
void Packet::SetExpireTime(int milliseconds)
{
    struct timeval t;
    gettimeofday(&t, NULL);
    if (milliseconds == 0) {
        milliseconds = 1000*86400;  //һ��ĺ���ֵ
    }

    expireTime_ = (static_cast<int64_t>(t.tv_sec) * static_cast<int64_t>(1000000) 
	+ static_cast<int64_t>(t.tv_usec)
	+ static_cast<int64_t>(milliseconds) * static_cast<int64_t>(1000));
}

