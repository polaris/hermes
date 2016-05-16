#ifndef _Protocol_H
#define _Protocol_H

#include <boost/asio.hpp>

class Packet;
class MoveList;
class PeerRegistry;

const uint32_t   PROTOCOL_MAGIC_NUMBER          = 0x01600CE8;

const uint8_t    PROTOCOL_VERSION               = 0x01;

const uint32_t   PROTOCOL_INVALID_PLAYER_ID     = 0;
const uint32_t   PROTOCOL_INVALID_OBJECT_ID     = 0;
const uint32_t   PROTOCOL_INVALID_CLASS_ID      = 0;

const uint8_t    PROTOCOL_PACKET_TYPE_INVALID   = 0x00;
const uint8_t    PROTOCOL_PACKET_TYPE_HELLO     = 0x01;
const uint8_t    PROTOCOL_PACKET_TYPE_WELCOME   = 0x02;
const uint8_t    PROTOCOL_PACKET_TYPE_INPUT     = 0x03;
const uint8_t    PROTOCOL_PACKET_TYPE_STATE     = 0x04;
const uint8_t    PROTOCOL_PACKET_TYPE_TICK      = 0x05;
const uint8_t    PROTOCOL_PACKET_TYPE_TOCK      = 0x06;
const uint8_t    PROTOCOL_PACKET_TYPE_INVITE    = 0x07;
const uint8_t    PROTOCOL_PACKET_TYPE_INTRO     = 0x08;

const float      PROTOCOL_HELLO_INTERVAL        = 1.0f;
const float      PROTOCOL_INPUT_INTERVAL        = 0.033f;

const float      PROTOCOL_CLIENT_TIMEOUT        = 1.0f;

void createHelloPacket(Packet* packet, const boost::asio::ip::udp::endpoint& endpoint);
void createWelcomePacket(Packet* packet, uint32_t playerId, uint32_t objectId, const boost::asio::ip::udp::endpoint& endpoint);
void createInputPacket(Packet* packet, uint32_t playerId, const boost::asio::ip::udp::endpoint& endpoint, const MoveList& moveList);
void createStatePacket(Packet* packet, const boost::asio::ip::udp::endpoint& endpoint);
void createTickPacket(Packet* packet, uint32_t playerId, float timeStamp, const boost::asio::ip::udp::endpoint& endpoint);
void createTockPacket(Packet* packet, float timeStamp, const boost::asio::ip::udp::endpoint& endpoint);
void createInvitePacket(Packet* packet, uint32_t playerId, const boost::asio::ip::udp::endpoint& endpoint, const PeerRegistry& peerRegistry);
void createIntroPacket(Packet* packet, uint32_t newPeerPlayerId, const boost::asio::ip::udp::endpoint& newPeerEndpoint, const boost::asio::ip::udp::endpoint& endpoint);

#endif  // _Protocol_H
