#include "GameServer.h"
#include "Renderer.h"
#include "Clock.h"
#include "Protocol.h"
#include "SpaceShip.h"

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

    checkForDisconnects(clock);

    // respawn cats
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
    if (!hasClientSession(packet->getEndpoint())) {
        BOOST_LOG_TRIVIAL(debug) << "HELLO from new client " << packet->getEndpoint();
        auto welcomePacket = packetPool_.pop();
        if (welcomePacket) {
            const auto playerId = nextPlayerId_++;
            const auto objectId = nextObjectId_++;
            GameObjectPtr gameObject(new SpaceShip(playerId, objectId, renderer_));
            world_.add(objectId, gameObject);
            createNewClientSession(playerId, packet->getEndpoint(), clock);
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
    if (verifyClientSession(playerId, packet->getEndpoint())) {
        clientSessions_[playerId]->handleInput(packet, clock);
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Received INPUT from unknown or invalid client " << packet->getEndpoint();
    }
}

void GameServer::handleGoodBye(Packet* packet) {
    unsigned int playerId = PROTOCOL_INVALID_PLAYER_ID;
    packet->read(playerId);
    if (verifyClientSession(playerId, packet->getEndpoint())) {
        BOOST_LOG_TRIVIAL(debug) << "Received GOODBYE from client " << playerId;
        removeClientSession(playerId);
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Received GOODBYE from unknown or invalid client " << packet->getEndpoint();
    }
}

void GameServer::checkForDisconnects(const Clock& clock) {
    std::vector<unsigned int> clientsToBeRemoved;
    for (const auto& clientSession : clientSessions_) {
        const auto now = clock.getTime();
        if ((now - clientSession.second->getLastSeen()) > PROTOCOL_CLIENT_TIMEOUT) {
            clientsToBeRemoved.push_back(clientSession.first);
        }
    }
    for (const auto& playerId : clientsToBeRemoved) {
        BOOST_LOG_TRIVIAL(debug) << "Remove disconnected client " << playerId;
        removeClientSession(playerId);
    }
}

void GameServer::handleDidUpdateWorld(const Clock&) {
    renderer_.clear(0.25f, 0.25f, 0.25f);
    world_.draw(renderer_);
    renderer_.present();

    // send outgoing packets
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

void GameServer::createNewClientSession(unsigned int playerId, const boost::asio::ip::udp::endpoint& endpoint, const Clock& clock) {
    auto newClientSession = std::make_unique<ClientSession>(endpoint, playerId, clock.getTime());
    clientSessions_.insert(PlayerIdToSessionMap::value_type(playerId, std::move(newClientSession)));

    const auto endpointAsString = boost::lexical_cast<std::string>(endpoint);
    playerIds_.insert(EndpointToPlayerIdMap::value_type(endpointAsString, playerId));
}

bool GameServer::verifyClientSession(unsigned int playerId, const boost::asio::ip::udp::endpoint& endpoint) {
    if (clientSessions_.find(playerId) != clientSessions_.end()) {
        const auto endpointAsString = boost::lexical_cast<std::string>(endpoint);
        return playerIds_[endpointAsString] == playerId;
    }
    return false;
}

bool GameServer::hasClientSession(const boost::asio::ip::udp::endpoint& endpoint) {
    return playerIds_.find(boost::lexical_cast<std::string>(endpoint)) != playerIds_.end();
}

void GameServer::removeClientSession(unsigned int playerId) {
    auto itr = clientSessions_.find(playerId);
    const auto endpointAsString = boost::lexical_cast<std::string>(itr->second->getEndpoint());
    playerIds_.erase(playerIds_.find(endpointAsString));
    clientSessions_.erase(itr);
}
