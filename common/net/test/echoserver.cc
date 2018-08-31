#include "common/net/qynet.h"
#include <signal.h>

#define DATA_MAX_SIZE 4096

class EchoPacket : public Packet
{
public:
    EchoPacket() {
        str_[0] = '\0';
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
        //LOG_INFO << "WRITE_DATA: " <<  str_;
        return true;
    }

    bool Decode(evpp::Buffer *input, PacketHeader *header)
    {
        int len = header->dataLen_;
        if (len >= DATA_MAX_SIZE) {
            len = DATA_MAX_SIZE - 1;
        }
        std::string str = input->NextString(len);
        strncpy(str_, str.c_str(), DATA_MAX_SIZE);
        //LOG_INFO << "READ_DATA:" << str_;
        str_[21]='a';
        str_[len] = '\0';
        return true;
    }
        
private:
    char str_[DATA_MAX_SIZE];
};

class EchoPacketFactory : public IPacketFactory
{
public:
    Packet *CreatePacket(int /*pcode*/)
    {
        return new EchoPacket();
    }
};

class EchoServerAdapter : public IServerAdapter
{
public:
    IPacketHandler::HPRetCode HandlePacket(ServerConnection *connection, Packet *packet)
    {
        EchoPacket *reply = new EchoPacket();
        reply->SetString(((EchoPacket*)packet)->GetString());
        reply->SetChannelId(packet->GetChannelId());
        if (connection->PostPacket(reply) == false) {
            reply->Free();
        }
        packet->Free();
        return IPacketHandler::FREE_CHANNEL;
    }    
};

ServerBase *echoServer_;
void singalHandler(int sig)
{
    if (sig == SIGTERM) {
    	echoServer_->Stop();
    } else {
        LOG_ERROR << "unexpected signal: " << sig;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("%s ip:port\n", argv[0]);
        return -1;
    }
    evpp::EventLoop loop;
    EchoPacketFactory factory;
    DefaultPacketStreamer streamer(&factory);
    EchoServerAdapter serverAdapter;
    uint32_t thread_num = 4;
    std::string addr(argv[1]);
    std::string name = "echo server";
    ServerBase echoServer(&loop, &streamer, &serverAdapter, addr, name, thread_num);
    signal(SIGTERM, singalHandler);
    echoServer_ = &echoServer;
    echoServer.Start();
    return 0;
}
