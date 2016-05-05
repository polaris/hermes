#include "GameClient.h"
#include "Renderer.h"
#include "Clock.h"
#include "Protocol.h"
#include "GameObjectRegistry.h"
#include "SpaceShip.h"
#include "Packet.h"

#include <spdlog/spdlog.h>
#include <boost/format.hpp>

GameClient::GameClient(unsigned int frameRate, const char *address, uint16_t port, Renderer& renderer)
: Game(frameRate, renderer)
, inputHandler_(frameRate)
, localSpaceShip_(nullptr)
, currentState(new GameClient::Connecting{this})
, nextState(nullptr)
, bufferedQueue_(100)
, transceiver_(bufferedQueue_)
, serverEndpoint_(boost::asio::ip::address::from_string(address), port)
, playerId_(PROTOCOL_INVALID_PLAYER_ID) {
}

void GameClient::handleWillUpdateWorld(const Clock& clock) {
    currentState->handleWillUpdateWorld(clock);
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
    auto packet = bufferedQueue_.dequeue();
    if (packet) {
        uint32_t magicNumber = 0;
        packet->read(magicNumber);
        if (magicNumber == PROTOCOL_MAGIC_NUMBER) {
            unsigned char protocolVersion = 0;
            packet->read(protocolVersion);
            if (protocolVersion == PROTOCOL_VERSION) {
                currentState->handleIncomingPacket(packet);
            } else {
                const auto logMessage = boost::str(boost::format("Received a packet from server with invalid protocol version %1%.") % protocolVersion);
                spdlog::get("console")->warn(logMessage);
            }
        } else {
            const auto logMessage = boost::str(boost::format("Received an invalid packet from %1%.") % packet->getEndpoint());
            spdlog::get("console")->info(logMessage);
        }
        bufferedQueue_.push(packet);
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

void GameClient::Connecting::handleWillUpdateWorld(const Clock&) {
}

void GameClient::Connecting::handleIncomingPacket(Packet* packet) {
    unsigned char packetType = PROTOCOL_PACKET_TYPE_INVALID;
    packet->read(packetType);
    switch (packetType) {
    case PROTOCOL_PACKET_TYPE_WELCOME:
        handleWelcome(packet);
        break;
    default:
        const auto logMessage = boost::str(boost::format("Received a packet with unexpected packet type %1% from %2%.") % static_cast<unsigned int>(packetType) % packet->getEndpoint());
        spdlog::get("console")->warn(logMessage);
        break;
    }
}

void GameClient::Connecting::handleWelcome(Packet* packet) {
    packet->read(gameClient_->playerId_);
    const auto logMessage = boost::str(boost::format("WELCOME received from %1%. My player ID is %2%.") % packet->getEndpoint() % gameClient_->playerId_);
    spdlog::get("console")->debug(logMessage);
    auto newState = std::shared_ptr<State>(new Connected{gameClient_});
    gameClient_->setState(newState);
}

void GameClient::Connecting::sendOutgoingPackets(const Clock& clock) {
    sendHello(clock);
}

void GameClient::Connecting::sendHello(const Clock& clock) {
    const auto now = clock.getTime();
    if (now > lastHelloTime_ + PROTOCOL_HELLO_INTERVAL) {
        auto packet = gameClient_->bufferedQueue_.pop();
        if (packet) {
            createHelloPacket(packet, gameClient_->serverEndpoint_);
            const auto logMessage = boost::str(boost::format("Send HELLO to %1%.") % packet->getEndpoint());
            spdlog::get("console")->debug(logMessage);
            gameClient_->transceiver_.sendTo(packet);
        } else {
            // TODO: Count failures and go to error state after a reasonable number of tries.
            spdlog::get("console")->warn("Failed to send HELLO to server: empty packet pool.");
        }
        lastHelloTime_ = now;
    }
}

GameClient::Connected::Connected(GameClient* gameClient)
: State(gameClient)
, lastInputTime_(0)
, lastTickTime_(0)
, objectIdToGameObjectMap_() {
}

void GameClient::Connected::handleWillUpdateWorld(const Clock& clock) {
    gameClient_->inputHandler_.update(clock.getGameTime());

    if (gameClient_->localSpaceShip_) {
        auto move = gameClient_->inputHandler_.getAndClearPendingMove();
        if (move) {
            const auto& inputState = move->getInputState();
            gameClient_->localSpaceShip_->rotate(inputState.desiredRightAmount * gameClient_->frameDuration_);
            gameClient_->localSpaceShip_->rotate(-inputState.desiredLeftAmount * gameClient_->frameDuration_);
            gameClient_->localSpaceShip_->thrust(inputState.desiredForwardAmount > 0);
        }
    }
}

void GameClient::Connected::handleIncomingPacket(Packet* packet) {
    unsigned char packetType = PROTOCOL_PACKET_TYPE_INVALID;
    packet->read(packetType);
    switch (packetType) {
    case PROTOCOL_PACKET_TYPE_STATE:
        handleState(packet);
        break;
    default:
        const auto logMessage = boost::str(boost::format("Received a packet with unexpected packet type %1% from %2%.") % static_cast<unsigned int>(packetType) % packet->getEndpoint());
        spdlog::get("console")->warn(logMessage);
        break;
    }
}

void GameClient::Connected::handleState(Packet* packet) {
    uint32_t gameObjectCount = 0;
    packet->read(gameObjectCount);
    for (uint32_t i = 0; i < gameObjectCount; i++) {
        uint32_t objectId = 0, classId = 0;
        packet->read(objectId);
        packet->read(classId);
        GameObject* gameObject = gameClient_->world_.getGameObject(objectId);
        if (gameObject == nullptr) {
            auto gameObjectPtr = GameObjectRegistry::get().createGameObject(classId, gameClient_->renderer_);
            gameClient_->world_.add(objectId, gameObjectPtr);
            gameObject = gameObjectPtr.get();
            gameClient_->localSpaceShip_ = std::dynamic_pointer_cast<SpaceShip>(gameObjectPtr);
        }
        gameObject->read(packet);
    }
}

void GameClient::Connected::sendOutgoingPackets(const Clock& clock) {
    const auto now = clock.getTime();
    if (now > lastInputTime_ + gameClient_->inputHandler_.getSampleInterval()) {
        if (sendInput()) {
            lastInputTime_ = now;
        }
    }

    if (now > lastInputTime_ + 0.5f && now > lastTickTime_ + 0.5f) {
        if (sendTick()) {
            lastTickTime_ = now;
        }
    }
}

bool GameClient::Connected::sendInput() {
    auto& moveList = gameClient_->inputHandler_.getMoveList();
    if (moveList.getCount() > 0) {
        auto packet = gameClient_->bufferedQueue_.pop();
        if (packet) {
            createInputPacket(packet, gameClient_->playerId_, gameClient_->serverEndpoint_, moveList);
            moveList.clear();
            gameClient_->transceiver_.sendTo(packet);
            return true;
        } else {
            // TODO: Count failures and go to error state after a reasonable number of tries.
            spdlog::get("console")->warn("Failed to send INPUT to server: empty packet pool.");
        }
    }
    return false;
}

bool GameClient::Connected::sendTick() {
    auto packet = gameClient_->bufferedQueue_.pop();
    if (packet) {
        createTickPacket(packet, gameClient_->playerId_, gameClient_->serverEndpoint_);
        gameClient_->transceiver_.sendTo(packet);
        return true;
    } else {
        // TODO: Count failures and go to error state after a reasonable number of tries.
        spdlog::get("console")->warn("Failed to send INPUT to server: empty packet pool.");
    }
    return false;
}
