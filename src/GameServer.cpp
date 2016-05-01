#include "GameServer.h"
#include "Renderer.h"
#include "Clock.h"
#include "Protocol.h"
#include "ServerSpaceShip.h"
#include "ClientSession.h"
#include "Packet.h"

#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>

GameServer::GameServer(unsigned int frameRate, uint16_t port, Renderer& renderer)
: Game(frameRate, renderer)
, nextPlayerId_(1)
, nextObjectId_(1)
, packetPool_(100, [] () { return new Packet(1500); })
, incomingPackets_(100)
, transceiver_(port, packetPool_, incomingPackets_) {
}

void GameServer::handleWillUpdateWorld(const Clock& clock) {
    processIncomingPackets(clock);
    clientRegistry_.checkForDisconnects(clock.getTime(), [this] (uint32_t playerId) {
        auto itr = playerToObjectMap_.find(playerId);
        if (itr != playerToObjectMap_.end()) {
            const uint32_t objectId = itr->second;
            world_.remove(objectId);
            playerToObjectMap_.erase(itr);
        }
    });
}

void GameServer::processIncomingPackets(const Clock& clock) {
    auto packet = incomingPackets_.pop();
    if (packet) {
        uint32_t magicNumber = 0;
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

            ClientSession* clientSession = clientRegistry_.addClientSession(playerId, packet->getEndpoint(), clock.getTime());

            SpaceShipPtr newSpaceShip(new ServerSpaceShip(renderer_, clientSession));
            newSpaceShip->setPlayerId(playerId);

            auto gameObject = std::dynamic_pointer_cast<GameObject>(newSpaceShip);
            world_.add(objectId, gameObject);
            playerToObjectMap_[playerId] = objectId;

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
    uint32_t playerId = PROTOCOL_INVALID_PLAYER_ID;
    packet->read(playerId);
    if (clientRegistry_.verifyClientSession(playerId, packet->getEndpoint())) {
        auto clientSession = clientRegistry_.getClientSession(playerId);
        clientSession->setLastSeen(clock.getTime());
        MoveList& moveList = clientSession->getMoveList();
        uint32_t count = 0;
        packet->read(count);
        for (uint32_t i = 0; i < count; i++) {
            Move move;
            move.read(packet);
            moveList.addMove(move);
        }
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Received INPUT from unknown or invalid client " << packet->getEndpoint();
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
    clientRegistry_.forEachSession([this] (ClientSession* clientSession) {
        auto packet = packetPool_.pop();
        if (packet) {
            createStatePacket(packet, clientSession->getEndpoint());

            packet->write(world_.getGameObjectCount());
            world_.forEachGameObject([this, packet] (uint32_t objectId, GameObject* gameObject) {
                packet->write(objectId);
                packet->write(gameObject->getClassId());
                gameObject->write(packet);
            });

            transceiver_.sendTo(packet);
        } else {
            BOOST_LOG_TRIVIAL(warning) << "Failed to WELCOME to client: empty packet pool";
        }
    });
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
