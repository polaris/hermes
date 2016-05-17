#include "Protocol.h"
#include "Packet.h"
#include "MoveList.h"
#include "PeerRegistry.h"

#include <boost/lexical_cast.hpp>

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

void createInvitePacket(Packet* packet, uint32_t playerId, const boost::asio::ip::udp::endpoint& endpoint, const PeerRegistry& peerRegistry) {
    createBasicPacket(packet, endpoint, PROTOCOL_PACKET_TYPE_INVITE);
    packet->write(playerId);
    packet->write(peerRegistry.getCount());
    peerRegistry.forEachPeer([packet] (const Peer& peer) {
        packet->write(boost::lexical_cast<std::string>(peer.first.address()));
        packet->write(peer.first.port());
        packet->write(peer.second);
    });
}

void createIntroPacket(Packet* packet, uint32_t newPeerPlayerId, const boost::asio::ip::udp::endpoint& newPeerEndpoint, const boost::asio::ip::udp::endpoint& endpoint) {
    createBasicPacket(packet, endpoint, PROTOCOL_PACKET_TYPE_INTRO);
    packet->write(newPeerPlayerId);
    packet->write(boost::lexical_cast<std::string>(newPeerEndpoint.address()));
    packet->write(newPeerEndpoint.port());
}

void createInputPacket(Packet* packet, uint32_t playerId, const boost::asio::ip::udp::endpoint& endpoint, const MoveList& moveList) {
    createBasicPacket(packet, endpoint, PROTOCOL_PACKET_TYPE_INPUT);
    packet->write(playerId);
    packet->write(moveList.getLatestTimeStamp());
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
