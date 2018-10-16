#pragma once

#include <map>
#include <list>
#include <mutex>
#include <atomic>

#define CHANNEL_CLUSTER_SIZE 25

class Channel;

class ChannelPool
{
public:
    ChannelPool();

    ~ChannelPool();

    Channel* AllocChannel();

    bool FreeChannel(Channel *channel);
    bool AppendChannel(Channel *channel);

    Channel* OfferChannel(uint32_t id);

    Channel* GetTimeoutList(int64_t now);

    bool AppendFreeList(Channel *addList);

    int GetUseListCount()
    {
        return static_cast<int32_t>(useMap_.size());
    }

    void SetExpireTime(Channel *channel, int64_t now); 

private:
    typedef std::map<uint32_t, Channel*> UseMap;
    UseMap useMap_;
    std::list<Channel*> clusterList_;
    std::mutex mutex_;

    Channel *freeListHead_;
    Channel *freeListTail_;
    Channel *useListHead_;
    Channel *useListTail_;
    int maxUseCount_;

    static std::atomic<uint32_t> globalChannelId_;
    static std::atomic<int> globalTotalCount_;
};


