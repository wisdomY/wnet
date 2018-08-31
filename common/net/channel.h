#pragma once

#include <cstdint>

#define QYNET_MAX_TIME (1ll << 62)

class IPacketHandler;
class ChannelPool;

class Channel {
    friend class ChannelPool;

public:
    Channel();
    
    void SetId(uint32_t id);

    uint32_t GetId();

    void SetArgs(void *args);

    void *GetArgs();

    void SetHandler(IPacketHandler *handler);

    IPacketHandler *GetHandler();

    void SetExpireTime(int64_t timeout);

    int64_t GetExpireTime()
    {
        return expireTime_;
    }

    Channel *GetNext()
    {
        return next_;
    }

private:
    uint32_t id_;
    void *args_;
    IPacketHandler *handler_;
    int64_t expireTime_;

private:
    Channel *prev_;
    Channel *next_;
};
