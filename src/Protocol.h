#ifndef _Protocol_H
#define _Protocol_H

#include <boost/asio.hpp>

class Packet;
class MoveList;

const unsigned int   PROTOCOL_MAGIC_NUMBER          = 0x01600CE8;

const unsigned char  PROTOCOL_VERSION               = 0x01;

const unsigned int   PROTOCOL_INVALID_PLAYER_ID     = 0;
const unsigned int   PROTOCOL_INVALID_OBJECT_ID     = 0;

const unsigned char  PROTOCOL_PACKET_TYPE_INVALID   = 0x00;
const unsigned char  PROTOCOL_PACKET_TYPE_HELLO     = 0x01;
const unsigned char  PROTOCOL_PACKET_TYPE_WELCOME   = 0x02;
const unsigned char  PROTOCOL_PACKET_TYPE_INPUT     = 0x03;
const unsigned char  PROTOCOL_PACKET_TYPE_STATE     = 0x04;
const unsigned char  PROTOCOL_PACKET_TYPE_GOODBYE   = 0x05;

const float          PROTOCOL_HELLO_INTERVAL        = 1.0f;
const float          PROTOCOL_INPUT_INTERVAL        = 0.033f;

const float          PROTOCOL_CLIENT_TIMEOUT        = 1.0f;

void createHelloPacket(Packet* packet, const boost::asio::ip::udp::endpoint& endpoint);
void createWelcomePacket(Packet* packet, unsigned int playerId, const boost::asio::ip::udp::endpoint& endpoint);
void createInputPacket(Packet* packet, unsigned int playerId, const boost::asio::ip::udp::endpoint& endpoint, const MoveList& moveList);

#endif  // _Protocol_H
