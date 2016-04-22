#ifndef _Protocol_H
#define _Protocol_H

#include <boost/asio.hpp>

class Packet;

const unsigned int   PROTOCOL_MAGIC_NUMBER          = 0x01600CE8;

const unsigned char  PROTOCOL_VERSION               = 0x01;

const unsigned char  PROTOCOL_PACKET_TYPE_INVALID   = 0x00;
const unsigned char  PROTOCOL_PACKET_TYPE_HELLO     = 0x01;
const unsigned char  PROTOCOL_PACKET_TYPE_WELCOME   = 0x02;
const unsigned char  PROTOCOL_PACKET_TYPE_INPUT     = 0x03;
const unsigned char  PROTOCOL_PACKET_TYPE_STATE     = 0x04;

const float          PROTOCOL_HELLO_INTERVAL        = 1.0;

void createHelloPacket(Packet* packet, boost::asio::ip::udp::endpoint& endpoint);
void createWelcomePacket(Packet* packet, boost::asio::ip::udp::endpoint& endpoint);

#endif  // _Protocol_H
