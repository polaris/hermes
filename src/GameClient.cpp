#include "GameClient.h"
#include "Renderer.h"
#include "Clock.h"
#include "Protocol.h"

#include <boost/log/trivial.hpp>

GameClient::GameClient(unsigned int frameRate, const char *address, unsigned short port, Renderer& renderer)
: Game(frameRate, renderer)
, currentState(new GameClient::Connecting{this})
, packetPool_(100 , [] () { return new Packet(1500); })
, incomingPackets_(100)
, transceiver_(packetPool_, incomingPackets_)
, serverEndpoint_(boost::asio::ip::address::from_string(address), port) {
}

void GameClient::handleWillUpdateWorld(const Clock& clock) {
    inputHandler_.update(clock.getGameTime());
}

void GameClient::handleDidUpdateWorld(const Clock& clock) {
    currentState->processIncomingPackets(clock);
    renderFrame();
    currentState->sendOutgoingPackets(clock);
}

void GameClient::finishFrame() {
    if (nextState) {
        currentState = nextState;
        nextState.reset();
    }
}

void GameClient::setState(std::shared_ptr<State>& newState) {
    nextState = newState;
}

void GameClient::renderFrame() {
    renderer_.clear(0.25f, 0.25f, 0.25f);
    world_.draw(renderer_);
    renderer_.present();
}

GameClient::State::State(GameClient* gameClient)
: gameClient_(gameClient) {
}

GameClient::Connecting::Connecting(GameClient* gameClient)
: State(gameClient)
, lastHelloTime_(0) {
}

void GameClient::Connecting::processIncomingPackets(const Clock&) {
    auto packet = gameClient_->incomingPackets_.pop();
    if (packet) {
        unsigned int magicNumber = 0;
        packet->read(magicNumber);
        if (magicNumber == PROTOCOL_MAGIC_NUMBER) {
            handlePacket(packet);
        } else {
            BOOST_LOG_TRIVIAL(info) << "Received invalid packet from " << packet->getEndpoint();
        }
        gameClient_->packetPool_.push(packet);
    }
}

void GameClient::Connecting::sendOutgoingPackets(const Clock& clock) {
    sendHello(clock);
}

void GameClient::Connecting::handlePacket(Packet* packet) {
    unsigned char protocolVersion = 0;
    packet->read(protocolVersion);
    if (protocolVersion == PROTOCOL_VERSION) {
        unsigned char packetType = PROTOCOL_PACKET_TYPE_INVALID;
        packet->read(packetType);
        switch (packetType) {
        case PROTOCOL_PACKET_TYPE_WELCOME:
            handleWelcome(packet);
            break;
        default:
            BOOST_LOG_TRIVIAL(warning) << "Received a packet with unexpected packet type " << static_cast<unsigned int>(packetType) << " from " << packet->getEndpoint();
            break;
        }
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Received a packet from server with invalid protocol version " << static_cast<unsigned int>(protocolVersion);
    }
}

void GameClient::Connecting::handleWelcome(Packet* packet) {
    BOOST_LOG_TRIVIAL(debug) << "Received a WELCOME from " << packet->getEndpoint();
    auto newState = std::shared_ptr<State>(new Connected{gameClient_});
    gameClient_->setState(newState);
}

void GameClient::Connecting::sendHello(const Clock& clock) {
    auto now = clock.getTime();
    if (now > lastHelloTime_ + PROTOCOL_HELLO_INTERVAL) {
        auto packet = gameClient_->packetPool_.pop();
        if (packet) {
            createHelloPacket(packet, gameClient_->serverEndpoint_);
            BOOST_LOG_TRIVIAL(debug) << "Send HELLO to " << packet->getEndpoint();
            gameClient_->transceiver_.sendTo(packet);
        } else {
            // TODO: Count failures and go to error state after a reasonable number of tries.
            BOOST_LOG_TRIVIAL(warning) << "Failed to send HELLO to server: empty packet pool";
        }
        lastHelloTime_ = now;
    }
}

GameClient::Connected::Connected(GameClient* gameClient)
: State(gameClient) {
}

void GameClient::Connected::processIncomingPackets(const Clock&) {
}

void GameClient::Connected::sendOutgoingPackets(const Clock&) {
}
