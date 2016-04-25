#include "GameServer.h"
#include "Renderer.h"
#include "Clock.h"
#include "Protocol.h"
#include "SpaceShip.h"
#include "ClientSession.h"

#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>

GameServer::GameServer(unsigned int frameRate, unsigned short port, Renderer& renderer)
: Game(frameRate, renderer)
, nextPlayerId_(1)
, nextObjectId_(1)
, packetPool_(100, [] () { return new Packet(1500); })
, incomingPackets_(100)
, transceiver_(port, packetPool_, incomingPackets_) {
}

void GameServer::handleWillUpdateWorld(const Clock& clock) {
    processIncomingPackets(clock);
    clientRegistry_.checkForDisconnects(clock.getTime());
}

void GameServer::processIncomingPackets(const Clock& clock) {
    auto packet = incomingPackets_.pop();
    if (packet) {
        unsigned int magicNumber = 0;
        packet->read(magicNumber);
        if (magicNumber == PROTOCOL_MAGIC_NUMBER) {
            handlePacket(packet, clock);
        } else {
            BOOST_LOG_TRIVIAL(info) << "Received invalid packet from " << packet->getEndpoint();
        }
        packetPool_.push(packet);
    }
}

void GameServer::handlePacket(Packet* packet, const Clock& clock) {
    unsigned char protocolVersion = 0;
    packet->read(protocolVersion);
    if (protocolVersion == PROTOCOL_VERSION) {
        unsigned char packetType = PROTOCOL_PACKET_TYPE_INVALID;
        packet->read(packetType);
        switch (packetType) {
        case PROTOCOL_PACKET_TYPE_HELLO:
            handleHello(packet, clock);
            break;
        case PROTOCOL_PACKET_TYPE_INPUT:
            handleInput(packet, clock);
            break;
        case PROTOCOL_PACKET_TYPE_GOODBYE:
            handleGoodBye(packet);
            break;
        default:
            BOOST_LOG_TRIVIAL(warning) << "Received a packet with unexpected packet type " << static_cast<unsigned int>(packetType) << " from " << packet->getEndpoint();
            break;
        }
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Received a packet from client with invalid protocol version " << static_cast<unsigned int>(protocolVersion);
    }
}

void GameServer::handleHello(Packet* packet, const Clock& clock) {
    if (!clientRegistry_.hasClientSession(packet->getEndpoint())) {
        BOOST_LOG_TRIVIAL(debug) << "HELLO from new client " << packet->getEndpoint();
        auto welcomePacket = packetPool_.pop();
        if (welcomePacket) {
            const auto playerId = nextPlayerId_++;
            const auto objectId = nextObjectId_++;
            GameObjectPtr gameObject(new SpaceShip(playerId, objectId, renderer_));
            world_.add(objectId, gameObject);
            clientRegistry_.addClientSession(playerId, packet->getEndpoint(), clock.getTime());
            createWelcomePacket(welcomePacket, playerId, packet->getEndpoint());
            transceiver_.sendTo(welcomePacket);
            BOOST_LOG_TRIVIAL(debug) << "WELCOME client " << playerId << " (" << welcomePacket->getEndpoint() << ")";
        } else {
            BOOST_LOG_TRIVIAL(warning) << "Failed to WELCOME to client: empty packet pool";
        }
    } else {
        BOOST_LOG_TRIVIAL(warning) << "HELLO from already welcomed client " << packet->getEndpoint();
    }
}

void GameServer::handleInput(Packet* packet, const Clock& clock) {
    unsigned int playerId = PROTOCOL_INVALID_PLAYER_ID;
    packet->read(playerId);
    if (clientRegistry_.verifyClientSession(playerId, packet->getEndpoint())) {
        auto clientSession = clientRegistry_.getClientSession(playerId);
        clientSession->handleInput(packet, clock);
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Received INPUT from unknown or invalid client " << packet->getEndpoint();
    }
}

void GameServer::handleGoodBye(Packet* packet) {
    unsigned int playerId = PROTOCOL_INVALID_PLAYER_ID;
    packet->read(playerId);
    if (clientRegistry_.verifyClientSession(playerId, packet->getEndpoint())) {
        BOOST_LOG_TRIVIAL(debug) << "Received GOODBYE from client " << playerId;
        clientRegistry_.removeClientSession(playerId);
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Received GOODBYE from unknown or invalid client " << packet->getEndpoint();
    }
}

void GameServer::handleDidUpdateWorld(const Clock&) {
    renderWorld();
    sendOutgoingPackets();
}

void GameServer::renderWorld() {
    renderer_.clear(0.25f, 0.25f, 0.25f);
    world_.draw(renderer_);
    renderer_.present();
}

void GameServer::sendOutgoingPackets() {
    // auto packet = packetPool_.pop();
    // if (packet) {
    //     transceiver_.sendTo(packet);
    // } else {
    //     BOOST_LOG_TRIVIAL(warning) << "Failed to WELCOME to client: empty packet pool";
    // }
}

void GameServer::handleEvent(SDL_Event& event, bool& running) {
    switch(event.type) {
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            running = false;
        }
        break;
    default:
        break;
    }
}
