#include <assert.h>

#include "evpp/logging.h"

#include "channel.h"
#include "channelpool.h"

std::atomic<uint32_t> ChannelPool::globalChannelId_ = {1};
std::atomic<int> ChannelPool::globalTotalCount_ = {0};

ChannelPool::ChannelPool()
    : freeListHead_(NULL)
    , freeListTail_(NULL)
    , useListHead_(NULL)
    , useListTail_(NULL)
    , maxUseCount_(0)
{
}

ChannelPool::~ChannelPool()
{
    for (std::list<Channel*>::iterator it = clusterList_.begin(); it != clusterList_.end(); it++) {
        delete[] *it;
    }
}

Channel *ChannelPool::AllocChannel()
{
    Channel *channel = NULL;

    std::lock_guard<std::mutex> guard(mutex_);
    if (freeListHead_ == NULL) {
        assert(CHANNEL_CLUSTER_SIZE > 2);
        Channel *channelCluster = new Channel[CHANNEL_CLUSTER_SIZE];
        globalTotalCount_ += CHANNEL_CLUSTER_SIZE;
        clusterList_.push_back(channelCluster);
        freeListHead_ = freeListTail_ = &channelCluster[1];
        for (int i = 2; i < CHANNEL_CLUSTER_SIZE; i++) {
            freeListTail_->next_ = &channelCluster[i];
            channelCluster[i].prev_ = freeListTail_;
            freeListTail_ = freeListTail_->next_;
        }
        freeListHead_->prev_ = NULL;
        freeListTail_->next_ = NULL;
        channel = &channelCluster[0];
    } else {
        channel = freeListHead_;
        freeListHead_ = freeListHead_->next_;
        if (freeListHead_ != NULL) {
            freeListHead_->prev_ = NULL;
        } else {
            freeListTail_ = NULL;
        }
    }
    
    channel->prev_ = useListTail_;
    channel->next_ = NULL;
    channel->expireTime_ = QYNET_MAX_TIME + 1;
    if (useListTail_ == NULL) {
        useListHead_ = channel;
    } else {
        useListTail_->next_ = channel;
    }
    useListTail_ = channel;

    uint32_t id = ChannelPool::globalChannelId_.fetch_add(1);
    id = (id & 0x0FFFFFFF);
    if (id == 0) {
      id = 1;
      ChannelPool::globalChannelId_.store(1);
    }
    channel->id_ = id;
    channel->handler_ = NULL;
    channel->args_ = NULL;

    useMap_[id] = channel;
    if (maxUseCount_ < (int)useMap_.size()) {
        maxUseCount_ = useMap_.size();
    }

    return channel;
}

bool ChannelPool::FreeChannel(Channel *channel)
{
    if (channel == NULL) {
        return false;
    }

    std::lock_guard<std::mutex> guard(mutex_);
    auto it = useMap_.find(channel->id_);
    if (it == useMap_.end()) {
        LOG_WARN << "Channel " << channel->id_ << " not in use map";
        return false;
    }
    useMap_.erase(it);

    //������������ɾ��channel
    if (channel == useListHead_) { // head
        useListHead_ = channel->next_;
    }
    if (channel == useListTail_) { // tail
        useListTail_ = channel->prev_;
    }
    if (channel->prev_ != NULL)
        channel->prev_->next_ = channel->next_;
    if (channel->next_ != NULL)
        channel->next_->prev_ = channel->prev_;

    //��channel�ӵ���������β��
    channel->prev_ = freeListTail_;
    channel->next_ = NULL;
    if (freeListTail_ == NULL) {
        freeListHead_ = channel;
    } else {
        freeListTail_->next_ = channel;
    }
    freeListTail_ = channel;

    //�ͷź�chennel�е�ֵ����
    channel->id_ = 0;
    channel->handler_ = NULL;
    channel->args_ = NULL;

    return true;
}


//����һ������channel��channelȡ������Ҫ���ô˷���
bool ChannelPool::AppendChannel(Channel *channel)
{
    std::lock_guard<std::mutex> _guard(mutex_);
    channel->prev_ = freeListTail_;
    channel->next_ = NULL;
    if (freeListTail_ == NULL) {
        freeListHead_ = channel;
    } else {
        freeListTail_->next_ = channel;
    }
    freeListTail_ = channel;
    channel->id_ = 0;
    channel->handler_ = NULL;
    channel->args_ = NULL;

    return true;
}


//ȡ��һ�����õ�channel
Channel *ChannelPool::OfferChannel(uint32_t id)
{
    Channel *channel = NULL;
    std::lock_guard<std::mutex> _guard(mutex_);
    auto it = useMap_.find(id);
    if (it != useMap_.end()) {
        channel = it->second;
        useMap_.erase(it);

        if (channel == useListHead_) { // head
            useListHead_ = channel->next_;
        }
        if (channel == useListTail_) { // tail
            useListTail_ = channel->prev_;
        }
        if (channel->prev_ != NULL)
            channel->prev_->next_ = channel->next_;
        if (channel->next_ != NULL)
            channel->next_->prev_ = channel->prev_;
        channel->prev_ = NULL;
        channel->next_ = NULL;
    }

    return channel;
}


//�����г�ʱchannelȡ��
Channel* ChannelPool::GetTimeoutList(int64_t now)
{
    Channel *list = NULL;

    std::lock_guard<std::mutex> _guard(mutex_);;
    if (useListHead_ == NULL) {
        return list;
    }

    Channel *channel = useListHead_;
    while (channel != NULL) {
        if (channel->expireTime_ >= now) break;//��ʱʱ�������ʱ�䳤��˵��δ��ʱ
        useMap_.erase(channel->id_);
        channel = channel->next_;
    }

    if (channel != useListHead_) {
        list = useListHead_;
        if (channel == NULL) {
            useListHead_ = useListTail_ = NULL;
        } else {
            if (channel->prev_ != NULL) {
                channel->prev_->next_ = NULL;
            }
            channel->prev_ = NULL;
            useListHead_ = channel;
        }
    }

    return list;
}


//���ӿ���channel�б�Ӧ����Ҫ���ڳ�ʱchannel
bool ChannelPool::AppendFreeList(Channel *addList) {
    if (addList == NULL) {
        return true;
    }

    std::lock_guard<std::mutex> _guard(mutex_);

    Channel *tail = addList;
    do {
        tail->id_ = 0;
        tail->handler_ = NULL;
        tail->args_ = NULL;
        tail = tail->next_;
    } while (tail != NULL);

    addList->prev_ = freeListTail_;
    if (freeListTail_ == NULL) {
        freeListHead_ = addList;
    } else {
        freeListTail_->next_ = addList;
    }
    freeListTail_ = tail;

    return true;
}


void ChannelPool::SetExpireTime(Channel *channel, int64_t now)
{
    if (channel == NULL) return;

    std::lock_guard<std::mutex> _guard(mutex_);
    channel->expireTime_ = now;
}
