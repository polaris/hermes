#include "GameClient.h"
#include "Renderer.h"
#include "Clock.h"
#include "Protocol.h"
#include "GameObjectRegistry.h"
#include "Packet.h"
#include "Logging.h"
#include "LocalSpaceShip.h"
#include "RemoteSpaceShip.h"
#include "RemoteLaserBolt.h"

#include <set>

GameClient::GameClient(unsigned int frameRate, unsigned int emulatedLatency, unsigned int stdDevLatencyMean, const char *address, uint16_t port, Renderer& renderer)
: Game(frameRate, renderer)
, world_()
, inputHandler_(30)
, latencyEstimator_(10)
, currentState(new GameClient::Connecting{this})
, nextState(nullptr)
, bufferedQueue_(1000)
, latencyEmulator_(bufferedQueue_, emulatedLatency, stdDevLatencyMean)
, transceiver_(latencyEmulator_)
, serverEndpoint_(boost::asio::ip::address::from_string(address), port)
, playerId_(PROTOCOL_INVALID_PLAYER_ID)
, objectId_(PROTOCOL_INVALID_OBJECT_ID) {
}

void GameClient::update(const Clock& clock) {
    currentState->handleWillUpdateWorld(clock);
    world_.update(frameDuration_);
    processIncomingPackets(clock);
    renderFrame();
    currentState->sendOutgoingPackets(clock);
    if (nextState) {
        currentState = nextState;
        nextState.reset();
    }
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
    GameObjectPtr gameObjectPtr;
    if (objectId == objectId_) {
        gameObjectPtr = std::shared_ptr<GameObject>(new LocalSpaceShip(renderer_, inputHandler_));
    } else {
        if (classId == SpaceShip::ClassId) {
            gameObjectPtr = std::shared_ptr<GameObject>(new RemoteSpaceShip(renderer_, latencyEstimator_, frameDuration_));
        } else if (classId == LaserBolt::ClassId) {
            gameObjectPtr = std::shared_ptr<GameObject>(new RemoteLaserBolt(renderer_, latencyEstimator_, frameDuration_));
        }
//        gameObjectPtr = GameObjectRegistry::get().createGameObject(classId, renderer_);
    }
    world_.add(objectId, gameObjectPtr);
    return gameObjectPtr.get();
}

GameClient::State::State(GameClient* gameClient)
: gameClient_(gameClient) {
}

GameClient::Connecting::Connecting(GameClient* gameClient)
: State(gameClient)
, lastHelloTime_(0) {
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
, objectIdToGameObjectMap_() {
}

void GameClient::Connected::handleWillUpdateWorld(const Clock& clock) {
    gameClient_->inputHandler_.update(clock.getGameTime());
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
    float latestInputTime = 0.0f;
    packet->read(latestInputTime);

    auto& moveList = gameClient_->inputHandler_.getMoveList();
    moveList.removeMovesUntil(latestInputTime);

    std::set<uint32_t> gameObjectsToRemove;
    gameClient_->world_.forEachGameObject([&gameObjectsToRemove] (uint32_t objectId, GameObject*) {
        gameObjectsToRemove.insert(objectId);
    });

    uint32_t gameObjectCount = 0;
    packet->read(gameObjectCount);
    for (uint32_t i = 0; i < gameObjectCount; i++) {
        uint32_t objectId = 0, classId = 0;
        packet->read(objectId);
        gameObjectsToRemove.erase(objectId);
        packet->read(classId);
        auto gameObject = gameClient_->world_.getGameObject(objectId);
        if (gameObject == nullptr) {
            gameObject = gameClient_->createNewGameObject(classId, objectId);
        }
        gameObject->read(packet);
    }

    for (const auto objectId : gameObjectsToRemove) {
        gameClient_->world_.remove(objectId);
    }
}

void GameClient::Connected::handleTock(Packet* packet, const Clock& clock) {
    float timeStamp = 0.0f;
    packet->read(timeStamp);
    const float roundTripTime = clock.getTime() - timeStamp;
    gameClient_->latencyEstimator_.addRTT(roundTripTime);
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
