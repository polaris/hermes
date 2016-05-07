#include "Protocol.h"
#include "Packet.h"
#include "MoveList.h"

static void createBasicPacket(Packet* packet, const boost::asio::ip::udp::endpoint& endpoint, uint8_t packetType) {
    packet->clear();
    packet->setEndpoint(endpoint);
    packet->write(PROTOCOL_MAGIC_NUMBER);
    packet->write(PROTOCOL_VERSION);
    packet->write(packetType);
}

void createHelloPacket(Packet* packet, const boost::asio::ip::udp::endpoint& endpoint) {
    createBasicPacket(packet, endpoint, PROTOCOL_PACKET_TYPE_HELLO);
}

void createWelcomePacket(Packet* packet, uint32_t playerId, uint32_t objectId, const boost::asio::ip::udp::endpoint& endpoint) {
    createBasicPacket(packet, endpoint, PROTOCOL_PACKET_TYPE_WELCOME);
    packet->write(playerId);
    packet->write(objectId);
}

void createInputPacket(Packet* packet, uint32_t playerId, const boost::asio::ip::udp::endpoint& endpoint, const MoveList& moveList) {
    createBasicPacket(packet, endpoint, PROTOCOL_PACKET_TYPE_INPUT);
    packet->write(playerId);
    moveList.write(packet);
}

void createStatePacket(Packet* packet, const boost::asio::ip::udp::endpoint& endpoint) {
    createBasicPacket(packet, endpoint, PROTOCOL_PACKET_TYPE_STATE);
}

void createTickPacket(Packet* packet, uint32_t playerId, float timeStamp, const boost::asio::ip::udp::endpoint& endpoint) {
    createBasicPacket(packet, endpoint, PROTOCOL_PACKET_TYPE_TICK);
    packet->write(playerId);
    packet->write(timeStamp);
}

void createTockPacket(Packet* packet, float timeStamp, const boost::asio::ip::udp::endpoint& endpoint) {
    createBasicPacket(packet, endpoint, PROTOCOL_PACKET_TYPE_TOCK);
    packet->write(timeStamp);
}
