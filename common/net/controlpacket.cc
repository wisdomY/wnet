#include "packet.h"
#include "controlpacket.h"

ControlPacket ControlPacket::BadPacket(CMD_BAD_PACKET);
ControlPacket ControlPacket::TimeoutPacket(CMD_TIMEOUT_PACKET);
ControlPacket ControlPacket::DisconnPacket(CMD_DISCONN_PACKET);
