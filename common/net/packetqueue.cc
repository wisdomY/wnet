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


//���������Ƶ�Ŀ�İ�������
//�����и�ע��㣬destQueue�����������еģ�������Ȼ���Է�����˽�г�Ա�뱣����Ա
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
            break;      //����Ӧ����continue��? ֻ�����а��ĳ�ʱʱ�����õ�һ�������break�ź��������������ٿ���
            //��ClientSession���У������ʱʱ���ȷ�����ó���ͬ��ʱ�䣬��������û�����⣬��Ϊ���Ķ��ж�����ͬ����Ϣ���ԣ���ʱʱ��ҲӦ�����ó�һ����
        }
        if (tail == NULL) {
            list = head_;
        }
        else {
            tail->next_ = head_;
        }
        tail = head_;

        head_ = head_->next_;
        
        //�о��������ifû�б�Ҫ
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


