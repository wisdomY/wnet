#include "evpp/logging.h"
#include "packetqueue.h"

PacketQueue::PacketQueue() : head_(NULL), tail_(NULL), size_(0)
{
}

PacketQueue::~PacketQueue()
{
    Clear();
}

Packet *PacketQueue::Pop()
{
    if (head_ == NULL) {
        return NULL;
    }
    Packet *packet = head_;
    head_ = head_->next_;
    if (head_ == NULL) {
        tail_ = NULL;
    }
    size_--;
    return packet;
}


void PacketQueue::Clear()
{
    if (head_ == NULL) {
        return;
    }
    while (head_ != NULL) {
        Packet *packet = head_;
        head_ = packet->next_;
        packet->Free();
    }
    head_ = tail_ = NULL;
    size_ = 0;
}


void PacketQueue::Push(Packet *packet)
{
    if (packet == NULL) {
        DLOG_TRACE << "Pused packet is null.";
        return;
    }
    packet->next_ = NULL;
    if (tail_ == NULL) {
        head_ = packet;
    }
    else {
        tail_->next_ = packet;
    }
    tail_ = packet;
    size_++;
}


int PacketQueue::Size()
{
    return size_;
}


bool PacketQueue::Empty()
{
    return (size_ == 0);
}


//将包队列移到目的包队列中
//这里有个注意点，destQueue对象是在类中的，所以仍然可以访问其私有成员与保护成员
void PacketQueue::MoveTo(PacketQueue *destQueue)
{
    if (head_ == NULL) {
        return;
    }
    if (destQueue->tail_ == NULL) {
        destQueue->head_ = head_;
    }
    else {
        destQueue->tail_->next_ = head_;
    }
    destQueue->tail_ = tail_;
    destQueue->size_ += size_;
    head_ = tail_ = NULL;
    size_ = 0;
}


Packet *PacketQueue::GetTimeoutList(int64_t now)
{
    Packet *list, *tail;
    list = tail = NULL;
    while (head_ != NULL) {
        int64_t t = head_->GetExpireTime();
        if (t == 0 || t >= now) {
            break;      //这里应该是continue吧? 只有所有包的超时时间设置的一样，这个break才合理，暂留，后期再看下
            //在ClientSession类中，这个超时时间的确被设置成相同的时间，所以这里没有问题，因为包的队列对于相同的消息而言，超时时间也应该设置成一样的
        }
        if (tail == NULL) {
            list = head_;
        }
        else {
            tail->next_ = head_;
        }
        tail = head_;

        head_ = head_->next_;
        
        //感觉下面这个if没有必要
        if (head_ == NULL) {
            tail_ = NULL;
        }
        size_ --;
    }
    if (tail) {
        tail->next_ = NULL;
    }
    return list;
}

Packet *PacketQueue::GetPacketList() {
    return head_;
}


