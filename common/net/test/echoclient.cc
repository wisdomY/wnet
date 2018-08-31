#include "common/net/qynet.h"
#include <signal.h>
int gsendcount = 1000;
int64_t gsendlen = 0;
ClientSession *echoClient;
int encode_count = 0;
std::atomic<bool> aborted;

#define DATA_MAX_SIZE 4096

class ClientEchoPacket : public Packet
{
public:
    ClientEchoPacket() {
        str_[0] = '\0';
        recvLen_ = 0;
    }
    
    void SetString(char *str) {
        strncpy(str_, str, DATA_MAX_SIZE);
        str_[DATA_MAX_SIZE-1] = '\0';
    }
    
    char *GetString() {
        return str_;
    }

    bool Encode(evpp::Buffer *output) {
        output->Append(str_, strlen(str_));
        encode_count ++;       
        return true;
    }

    bool Decode(evpp::Buffer *input, PacketHeader *header)
    {
        recvLen_ = header->dataLen_;
        int len = header->dataLen_;
        if (len >= DATA_MAX_SIZE) {
            len = DATA_MAX_SIZE - 1;
        }
        std::string str = input->NextString(len);
        strncpy(str_, str.c_str(), DATA_MAX_SIZE);
        str_[len] = '\0';
        if (header->dataLen_ > len) {
            input->Skip(header->dataLen_ - len);
        }
        return true;
    }
    
    int GetRecvLen() {
        return recvLen_;
    }

    void Free() {
    }
    
    int GetIndex() {return index_;}
    void SetIndex(int i) {index_ = i;}
        
private:
    char str_[DATA_MAX_SIZE];
    int recvLen_;
    int index_;
};

class ClientEchoPacketFactory : public IPacketFactory
{
public:
    Packet *CreatePacket(int /*pcode*/)
    {
        return new ClientEchoPacket();
    }
};

class ClientEchoPacketHandler : public IPacketHandler
{
public:
    ClientEchoPacketHandler() {
        recvlen_ = 0;
        timeoutCount_ = 0; 
        count_.store(0);
    }

    HPRetCode HandlePacket(Packet *packet, void *args)
    {
        ClientEchoPacket *echoPacket = (ClientEchoPacket*)args;
        count_++;
        if (!packet->IsRegularPacket()) {
            LOG_ERROR << "INDEX: " << echoPacket->GetIndex() << "  => ControlPacket: " << ((ControlPacket*)packet)->GetCommand();
            timeoutCount_++;
            delete echoPacket;
            return IPacketHandler::FREE_CHANNEL;
        }
        recvlen_ += ((ClientEchoPacket*)packet)->GetRecvLen();
        if (count_.load() == gsendcount) {
            LOG_INFO << "INDEX: " << echoPacket->GetIndex() << " OK=>_count: " <<  count_.load();        
            echoClient->Disconnect();
            echoClient->Stop(); 
        } else {
            LOG_INFO << "INDEX: " << echoPacket->GetIndex() << " OK=>_count: " <<  count_.load();        
        }
        delete echoPacket;
        delete packet;
        return IPacketHandler::FREE_CHANNEL;
    }    
private:
    std::atomic<int> count_;
    int64_t recvlen_;
    int timeoutCount_;
};

void singalHandler(int sig)
{
    if (sig == SIGTERM || sig == SIGINT) {
        LOG_INFO << "Received TERM/INT signal";
        aborted.store(true);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        echoClient->Disconnect();
        echoClient->Stop();
    }
}

void SendPacket(ClientSession *client) {
    char buffer[DATA_MAX_SIZE+1];
#ifndef WIN32
    int pid = getpid();
    LOG_INFO << "PID: " << pid;
#endif
    for(int i = 0; i < gsendcount; i++) {
        if (aborted.load()) break;
        int len = 1988;
#ifndef WIN32
        sprintf(buffer, "%010d_%010d", pid, i);
#endif
        memset(buffer + 21, 'a', len - 21);
        buffer[len] = '\0';
        ClientEchoPacket *packet = new ClientEchoPacket();
        packet->SetIndex(i + 1);
        packet->SetString(buffer);
        if (!client->PostPacket(packet, NULL, packet)) {
            LOG_ERROR << "PostPacket failed at " << (i + 1);
            break;
        }
        gsendlen += len;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("%s ip:port count\n", argv[0]);
        return -1;
    }
    aborted.store(false);
    int sendcount = atoi(argv[2]);
    if (sendcount > 0) {
        gsendcount = sendcount;
    }
    evpp::EventLoop loop;
    std::string raddr(argv[1]);
    ClientEchoPacketFactory factory;
    DefaultPacketStreamer streamer(&factory);
    ClientEchoPacketHandler handler;
    std::string name = "Echo Client";
    ClientSession client(&loop, &streamer, raddr, name);
    client.SetDefaultPacketHandler(&handler);
    client.SetQueueLimit(gsendcount);
    echoClient = &client;
    signal(SIGINT, singalHandler);
    signal(SIGTERM, singalHandler);
    client.Connect(SendPacket);
    loop.Run();
    return EXIT_SUCCESS;
}
