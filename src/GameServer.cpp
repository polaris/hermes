#include "GameServer.h"
#include "Renderer.h"
#include "Clock.h"
#include "Protocol.h"

#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>

GameServer::GameServer(unsigned int frameRate, unsigned short port, Renderer& renderer)
: Game(frameRate, renderer)
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
        BOOST_LOG_TRIVIAL(debug) << "Received HELLO from new client " << packet->getEndpoint();
        auto welcomePacket = packetPool_.pop();
        if (welcomePacket) {
            createNewClientSession(packet->getEndpoint(), clock);
            createWelcomePacket(welcomePacket, packet->getEndpoint());
            BOOST_LOG_TRIVIAL(debug) << "Send WELCOME to client " << welcomePacket->getEndpoint();
            transceiver_.sendTo(welcomePacket);
        } else {
            BOOST_LOG_TRIVIAL(warning) << "Failed to send WELCOME to client: empty packet pool";
        }
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Received HELLO from already welcomed client " << packet->getEndpoint();
    }
}

void GameServer::handleInput(Packet* packet, const Clock& clock) {
    if (hasClientSession(packet->getEndpoint())) {
        const auto id = boost::lexical_cast<std::string>(packet->getEndpoint());
        clientSessions_[id]->handleInput(packet, clock);
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Received INPUT from unknown client " << packet->getEndpoint();
    }
}

void GameServer::handleGoodBye(Packet* packet) {
    if (!hasClientSession(packet->getEndpoint())) {
        BOOST_LOG_TRIVIAL(debug) << "Received GOODBYE from client " << packet->getEndpoint();
        removeClientSession(packet->getEndpoint());
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Received GOODBYE from unknown client " << packet->getEndpoint();
    }
}

void GameServer::checkForDisconnects(const Clock& clock) {
    std::vector<std::string> clientsToBeRemoved;
    for (const auto& clientSession : clientSessions_) {
        const auto now = clock.getTime();
        if ((now - clientSession.second->getLastSeen()) > PROTOCOL_CLIENT_TIMEOUT) {
            clientsToBeRemoved.push_back(clientSession.first);
        }
    }
    for (const auto& id : clientsToBeRemoved) {
        BOOST_LOG_TRIVIAL(debug) << "Remove disconnected client " << id;
        clientSessions_.erase(clientSessions_.find(id));
    }
}

void GameServer::handleDidUpdateWorld(const Clock&) {
    renderer_.clear(0.25f, 0.25f, 0.25f);
    world_.draw(renderer_);
    renderer_.present();

    // send outgoing packets
}

void GameServer::createNewClientSession(const boost::asio::ip::udp::endpoint& endpoint, const Clock& clock) {
    auto id = boost::lexical_cast<std::string>(endpoint);
    auto newClientSession = std::make_unique<ClientSession>(endpoint, clock.getTime());
    clientSessions_.insert(ClientSessionMap::value_type(id, std::move(newClientSession)));
}

bool GameServer::hasClientSession(const boost::asio::ip::udp::endpoint& endpoint) {
    return clientSessions_.find(boost::lexical_cast<std::string>(endpoint)) != clientSessions_.end();
}

void GameServer::removeClientSession(const boost::asio::ip::udp::endpoint& endpoint) {
    clientSessions_.erase(clientSessions_.find(boost::lexical_cast<std::string>(endpoint)));
}
