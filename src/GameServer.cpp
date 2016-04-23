#include "GameServer.h"
#include "Renderer.h"
#include "Clock.h"
#include "Protocol.h"

#include <boost/log/trivial.hpp>

GameServer::GameServer(unsigned int frameRate, unsigned short port, Renderer& renderer)
: Game(frameRate, renderer)
, packetPool_(100, [] () { return new Packet(1500); })
, incomingPackets_(100)
, transceiver_(port, packetPool_, incomingPackets_) {
}

void GameServer::handleWillUpdateWorld(const Clock&) {
    processIncomingPackets();

    // check for disconnects

    // respawn cats
}

void GameServer::processIncomingPackets() {
    auto packet = incomingPackets_.pop();
    if (packet) {
        unsigned int magicNumber = 0;
        packet->read(magicNumber);
        if (magicNumber == PROTOCOL_MAGIC_NUMBER) {
            handlePacket(packet);
        } else {
            BOOST_LOG_TRIVIAL(info) << "Received invalid packet from " << packet->getEndpoint();
        }
        packetPool_.push(packet);
    }
}

void GameServer::handlePacket(Packet* packet) {
    unsigned char protocolVersion = 0;
    packet->read(protocolVersion);
    if (protocolVersion == PROTOCOL_VERSION) {
        unsigned char packetType = PROTOCOL_PACKET_TYPE_INVALID;
        packet->read(packetType);
        switch (packetType) {
        case PROTOCOL_PACKET_TYPE_HELLO:
            handleHello(packet);
            break;
        default:
            BOOST_LOG_TRIVIAL(warning) << "Received a packet with unexpected packet type " << static_cast<unsigned int>(packetType) << " from " << packet->getEndpoint();
            break;
        }
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Received a packet from client with invalid protocol version " << static_cast<unsigned int>(protocolVersion);
    }
}

void GameServer::handleHello(Packet* packet) {
    BOOST_LOG_TRIVIAL(debug) << "Received a HELLO from client " << packet->getEndpoint();
    auto welcomePacket = packetPool_.pop();
    if (welcomePacket) {
        createWelcomePacket(welcomePacket, packet->getEndpoint());
        BOOST_LOG_TRIVIAL(debug) << "Send WELCOME to client " << welcomePacket->getEndpoint();
        transceiver_.sendTo(welcomePacket);
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Failed to send WELCOME to client: empty packet pool";
    }
}

void GameServer::handleDidUpdateWorld(const Clock&) {
    renderer_.clear(0.25f, 0.25f, 0.25f);
    world_.draw(renderer_);
    renderer_.present();

    // send outgoing packets
}
