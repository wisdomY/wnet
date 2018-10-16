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


//设置包的超时时间，超时时间单位为us(但是此方法的参数单位为ms，因此需要*1000)，若参数传0，代表超时时间为24小时
void Packet::SetExpireTime(int milliseconds)
{
    struct timeval t;
    gettimeofday(&t, NULL);
    if (milliseconds == 0) {
        milliseconds = 1000*86400;  //一天的毫秒值
    }

    expireTime_ = (static_cast<int64_t>(t.tv_sec) * static_cast<int64_t>(1000000) 
	+ static_cast<int64_t>(t.tv_usec)
	+ static_cast<int64_t>(milliseconds) * static_cast<int64_t>(1000));
}

