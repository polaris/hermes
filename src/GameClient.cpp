#include "GameClient.h"
#include "Renderer.h"
#include "Clock.h"
#include "Protocol.h"
#include "GameObjectRegistry.h"
#include "Packet.h"
#include "Logging.h"

GameClient::GameClient(unsigned int frameRate, const char *address, uint16_t port, Renderer& renderer)
: Game(frameRate, renderer)
, inputHandler_(30)
, localSpaceShip_(nullptr)
, currentState(new GameClient::Connecting{this})
, nextState(nullptr)
, bufferedQueue_(1000)
, latencyEmulator_(bufferedQueue_, 150)
, transceiver_(latencyEmulator_)
, serverEndpoint_(boost::asio::ip::address::from_string(address), port)
, playerId_(PROTOCOL_INVALID_PLAYER_ID)
, objectId_(PROTOCOL_INVALID_OBJECT_ID) {
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

void GameClient::setState(StatePtr& newState) {
    nextState = newState;
}

void GameClient::processIncomingPackets(const Clock& clock) {
    auto packet = bufferedQueue_.dequeue();
    if (packet) {
        uint32_t magicNumber = 0;
        packet->read(magicNumber);
        if (magicNumber == PROTOCOL_MAGIC_NUMBER) {
            unsigned char protocolVersion = 0;
            packet->read(protocolVersion);
            if (protocolVersion == PROTOCOL_VERSION) {
                currentState->handleIncomingPacket(packet, clock);
            } else {
                WARN("Received a packet from server with invalid protocol version {0}.", protocolVersion);
            }
        } else {
            WARN("Received an invalid packet from {0}.", packet->getEndpoint());
        }
        bufferedQueue_.push(packet);
    }
}

void GameClient::renderFrame() {
    renderer_.clear(0.25f, 0.25f, 0.25f);
    world_.draw(renderer_);
    renderer_.present();
}

GameObject* GameClient::createNewGameObject(uint32_t classId, uint32_t objectId) {
    auto gameObjectPtr = GameObjectRegistry::get().createGameObject(classId, renderer_);
    world_.add(objectId, gameObjectPtr);
    if (objectId == objectId_) {
        localSpaceShip_ = std::dynamic_pointer_cast<SpaceShip>(gameObjectPtr);
    }
    return gameObjectPtr.get();
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

void GameClient::Connecting::handleIncomingPacket(Packet* packet, const Clock&) {
    unsigned char packetType = PROTOCOL_PACKET_TYPE_INVALID;
    packet->read(packetType);
    switch (packetType) {
    case PROTOCOL_PACKET_TYPE_WELCOME:
        handleWelcome(packet);
        break;
    default:
        WARN("Received a packet with unexpected packet type {0} from {1}.", static_cast<unsigned int>(packetType), packet->getEndpoint());
        break;
    }
}

void GameClient::Connecting::handleWelcome(Packet* packet) {
    packet->read(gameClient_->playerId_);
    packet->read(gameClient_->objectId_);
    DEBUG("WELCOME received from {0}. My player ID is {1}.", packet->getEndpoint(), gameClient_->playerId_);
    auto newState = StatePtr(new Connected{gameClient_});
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
            INFO("Send HELLO to {0}.", packet->getEndpoint());
            gameClient_->transceiver_.sendTo(packet);
        } else {
            WARN("Failed to send HELLO to server: empty packet pool.");
        }
        lastHelloTime_ = now;
    }
}

GameClient::Connected::Connected(GameClient* gameClient)
: State(gameClient)
, lastInputTime_(0)
, lastTickTime_(0)
, objectIdToGameObjectMap_()
, rollingMeanRrt_(10) {
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

void GameClient::Connected::handleIncomingPacket(Packet* packet, const Clock& clock) {
    unsigned char packetType = PROTOCOL_PACKET_TYPE_INVALID;
    packet->read(packetType);
    switch (packetType) {
    case PROTOCOL_PACKET_TYPE_STATE:
        handleState(packet);
        break;
    case PROTOCOL_PACKET_TYPE_TOCK:
        handleTock(packet, clock);
        break;
    default:
        WARN("Received a packet with unexpected packet type {0} from {1}.", static_cast<unsigned int>(packetType), packet->getEndpoint());
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
        auto gameObject = gameClient_->world_.getGameObject(objectId);
        if (gameObject == nullptr) {
            gameObject = gameClient_->createNewGameObject(classId, objectId);
        }
        gameObject->read(packet);
        const auto latency = rollingMeanRrt_.getMean() / 2.0f;
        gameObject->update(latency);
    }
}

void GameClient::Connected::handleTock(Packet* packet, const Clock& clock) {
    float timeStamp = 0.0f;
    packet->read(timeStamp);
    const float roundTripTime = clock.getTime() - timeStamp;
    rollingMeanRrt_.addValue(roundTripTime);
}

void GameClient::Connected::sendOutgoingPackets(const Clock& clock) {
    const auto now = clock.getTime();
    if (now > lastInputTime_ + gameClient_->inputHandler_.getSampleInterval()) {
        if (sendInput()) {
            lastInputTime_ = now;
        }
    }

    if (now > lastTickTime_ + 0.5f) {
        if (sendTick(clock)) {
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
            WARN("Failed to send INPUT to server: empty packet pool.");
        }
    }
    return false;
}

bool GameClient::Connected::sendTick(const Clock& clock) {
    auto packet = gameClient_->bufferedQueue_.pop();
    if (packet) {
        createTickPacket(packet, gameClient_->playerId_, clock.getTime(), gameClient_->serverEndpoint_);
        gameClient_->transceiver_.sendTo(packet);
        return true;
    } else {
        WARN("Failed to send INPUT to server: empty packet pool.");
    }
    return false;
}
