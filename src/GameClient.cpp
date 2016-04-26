#include "GameClient.h"
#include "Renderer.h"
#include "Clock.h"
#include "Protocol.h"
#include "GameObjectRegistry.h"
#include "SpaceShip.h"

#include <boost/log/trivial.hpp>

GameClient::GameClient(unsigned int frameRate, const char *address, unsigned short port, Renderer& renderer)
: Game(frameRate, renderer)
, inputHandler_(frameRate)
, currentState(new GameClient::Connecting{this})
, packetPool_(100 , [] () { return new Packet(1500); })
, incomingPackets_(100)
, transceiver_(packetPool_, incomingPackets_)
, serverEndpoint_(boost::asio::ip::address::from_string(address), port)
, playerId_(PROTOCOL_INVALID_PLAYER_ID) {
    auto gameObject = GameObjectRegistry::get().createGameObject(1, renderer_);
    world_.add(0, gameObject);

    localSpaceShip_ = std::dynamic_pointer_cast<SpaceShip>(gameObject);
}

void GameClient::handleWillUpdateWorld(const Clock& clock) {
    inputHandler_.update(clock.getGameTime());

    if (localSpaceShip_) {
        auto move = inputHandler_.getAndClearPendingMove();
        if (move) {
            const auto& inputState = move->getInputState();
            localSpaceShip_->rotate(inputState.desiredRightAmount * 5 * frameDuration_);
            localSpaceShip_->rotate(-inputState.desiredLeftAmount * 5 * frameDuration_);
            localSpaceShip_->thrust(inputState.desiredForwardAmount > 0);
        }
    }
}

void GameClient::handleDidUpdateWorld(const Clock& clock) {
    processIncomingPackets(clock);
    renderFrame();
    currentState->sendOutgoingPackets(clock);
}

void GameClient::handleEvent(SDL_Event& event, bool& running) {
    switch(event.type) {
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            running = false;
        } else {
            inputHandler_.handleInput(KeyAction::Down, event.key.keysym.sym);
        }
        break;
    case SDL_KEYUP:
        inputHandler_.handleInput(KeyAction::Up, event.key.keysym.sym);
        break;
    default:
        break;
    }
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

void GameClient::processIncomingPackets(const Clock&) {
    auto packet = incomingPackets_.pop();
    if (packet) {
        unsigned int magicNumber = 0;
        packet->read(magicNumber);
        if (magicNumber == PROTOCOL_MAGIC_NUMBER) {
            unsigned char protocolVersion = 0;
            packet->read(protocolVersion);
            if (protocolVersion == PROTOCOL_VERSION) {
                currentState->handleIncomingPacket(packet);
            } else {
                BOOST_LOG_TRIVIAL(warning) << "Received a packet from server with invalid protocol version " << static_cast<unsigned int>(protocolVersion);
            }
        } else {
            BOOST_LOG_TRIVIAL(info) << "Received invalid packet from " << packet->getEndpoint();
        }
        packetPool_.push(packet);
    }
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

void GameClient::Connecting::handleIncomingPacket(Packet* packet) {
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
}

void GameClient::Connecting::handleWelcome(Packet* packet) {
    packet->read(gameClient_->playerId_);
    BOOST_LOG_TRIVIAL(debug) << "WELCOME from " << packet->getEndpoint() << ". Player ID is " << gameClient_->playerId_ << ".";
    auto newState = std::shared_ptr<State>(new Connected{gameClient_});
    gameClient_->setState(newState);
}

void GameClient::Connecting::sendOutgoingPackets(const Clock& clock) {
    sendHello(clock);
}

void GameClient::Connecting::sendHello(const Clock& clock) {
    const auto now = clock.getTime();
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
: State(gameClient)
, lastInputTime_(0) {
}

void GameClient::Connected::handleIncomingPacket(Packet* packet) {
    unsigned char packetType = PROTOCOL_PACKET_TYPE_INVALID;
    packet->read(packetType);
    switch (packetType) {
    case PROTOCOL_PACKET_TYPE_STATE:
        handleState(packet);
        break;
    default:
        BOOST_LOG_TRIVIAL(warning) << "Received a packet with unexpected packet type " << static_cast<unsigned int>(packetType) << " from " << packet->getEndpoint();
        break;
    }
}

void GameClient::Connected::handleState(Packet* packet) {
    gameClient_->world_.read(packet);
}

void GameClient::Connected::sendOutgoingPackets(const Clock& clock) {
    const auto now = clock.getTime();
    if (now > lastInputTime_ + gameClient_->inputHandler_.getSampleInterval()) {
        sendInput();
        lastInputTime_ = now;
    }
}

void GameClient::Connected::sendInput() {
    auto& moveList = gameClient_->inputHandler_.getMoveList();
    if (moveList.getCount() > 0) {
        auto packet = gameClient_->packetPool_.pop();
        if (packet) {
            createInputPacket(packet, gameClient_->playerId_, gameClient_->serverEndpoint_, moveList);
            moveList.clear();
            gameClient_->transceiver_.sendTo(packet);
        } else {
            // TODO: Count failures and go to error state after a reasonable number of tries.
            BOOST_LOG_TRIVIAL(warning) << "Failed to send INPUT to server: empty packet pool";
        }
    }
}
