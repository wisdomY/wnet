#include "evpp/logging.h"

#include "defaultpacketstreamer.h"


int DefaultPacketStreamer::nPacketFlag_ = QYNET_PACKET_FLAG;

DefaultPacketStreamer::DefaultPacketStreamer()
{
}

DefaultPacketStreamer::DefaultPacketStreamer(IPacketFactory *factory) : IPacketStreamer(factory)
{
}

DefaultPacketStreamer::~DefaultPacketStreamer()
{
}

void DefaultPacketStreamer::SetPacketFactory(IPacketFactory *factory)
{
    factory_ = factory;
}

bool DefaultPacketStreamer::GetPacketInfo(evpp::Buffer *input, PacketHeader *header, bool *broken)
{
    if (existPacketHeader_) {
        if (input->length() < (int)(4 * sizeof(int))) {
            return false;
        }

        int flag = input->ReadInt32();
        header->chid_ = input->ReadInt32();
        header->pcode_ = input->ReadInt32();
        header->dataLen_ = input->ReadInt32();
        if (flag != DefaultPacketStreamer::nPacketFlag_ || header->dataLen_ < 0 ||
                header->dataLen_ > 0x4000000) { // 64M
            LOG_ERROR << "stream error: "<< std::hex << flag << "<>" << DefaultPacketStreamer::nPacketFlag_ << ", dataLen:" << header->dataLen_;
            *broken = true;
        }
    }
    else if (input->length() == 0) {
        return false;
    }
    return true;
}

Packet *DefaultPacketStreamer::Decode(evpp::Buffer *input, PacketHeader *header)
{
    assert(factory_ != NULL);
    Packet *packet = factory_->CreatePacket(header->pcode_);
    if (packet != NULL) {
        if (!packet->Decode(input, header)) {
            packet->Free();
            packet = NULL;
        }
    }
    else {
        input->Skip(header->dataLen_);
    }
    return packet;
}

bool DefaultPacketStreamer::Encode(Packet *packet, evpp::Buffer *output)
{
    PacketHeader *header = packet->GetPacketHeader();

    int oldLen = output->length();
    int dataLenOffset = -1;
    int headerSize = 0;

    if (existPacketHeader_) {
        output->AppendInt32(DefaultPacketStreamer::nPacketFlag_);
        output->AppendInt32(header->chid_);
        output->AppendInt32(header->pcode_);
        dataLenOffset = output->length();
        output->AppendInt32(0);
        headerSize = 4 * sizeof(int);
    }
    if (packet->Encode(output) == false) {
        LOG_ERROR << "encode error";
        output->Truncate(oldLen);
        return false;
    }
    header->dataLen_ = output->length() - oldLen - headerSize;
    if (dataLenOffset >= 0) {
        unsigned char *ptr = (unsigned char *)(output->data() + dataLenOffset);
        int32_t be32 = htonl(header->dataLen_);
        memcpy(ptr, &be32, sizeof(int32_t));
    }

    return true;
}

void DefaultPacketStreamer::SetPacketFlag(int flag) {
    DefaultPacketStreamer::nPacketFlag_ = flag;
}

