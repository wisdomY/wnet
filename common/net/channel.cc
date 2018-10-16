#include <cstddef>

#include "channel.h"

Channel::Channel() : prev_(NULL), next_(NULL), expireTime_(0)
{
}

void Channel::SetId(uint32_t id)
{
    id_ = id;
}

uint32_t Channel::GetId()
{
    return id_;
}

void Channel::SetArgs(void *args)
{
    args_ = args;
}

void *Channel::GetArgs()
{
    return args_;
}

void Channel::SetHandler(IPacketHandler *handler)
{
    handler_ = handler;
}

IPacketHandler *Channel::GetHandler()
{
    return handler_;
}

void Channel::SetExpireTime(int64_t expireTime)
{
    expireTime_ = expireTime;
}


