#include "GamePeer.h"
#include "Renderer.h"
#include "Protocol.h"
#include "Packet.h"
#include "Clock.h"
#include "Logging.h"
#include "LocalSpaceShip.h"
#include "RemoteSpaceShip.h"
#include "RemoteLaserBolt.h"

#include <set>

GamePeer::GamePeer(unsigned int width, unsigned int height, unsigned int frameRate, unsigned int updateRate, unsigned int emulatedLatency, unsigned int stdDevMeanLatency, Renderer& renderer, unsigned short port)
: Game(frameRate, renderer)
, world_(width, height)
, updateInterval_(1.0f/static_cast<float>(updateRate))
, inputHandler_(30)
, latencyEstimator_(10)
, currentState(nullptr)
, nextState(nullptr)
, peerRegistry_()
, playerId_(1)
, objectId_(PROTOCOL_INVALID_OBJECT_ID)
, bufferedQueue_(1000)
, latencyEmulator_(bufferedQueue_, emulatedLatency, stdDevMeanLatency)
, transceiver_(port, latencyEmulator_)
, masterPeerEndpoint_() {
    currentState.reset(new GamePeer::Accepting{this});
}

GamePeer::GamePeer(unsigned int width, unsigned int height, unsigned int frameRate, unsigned int updateRate, unsigned int emulatedLatency, unsigned int stdDevMeanLatency, Renderer& renderer, const char* address, unsigned short port)
: Game(frameRate, renderer)
, world_(width, height)
, updateInterval_(1.0f/static_cast<float>(updateRate))
, inputHandler_(30)
, latencyEstimator_(10)
, currentState(nullptr)
, nextState(nullptr)
, peerRegistry_()
, playerId_(PROTOCOL_INVALID_PLAYER_ID)
, objectId_(PROTOCOL_INVALID_OBJECT_ID)
, bufferedQueue_(1000)
, latencyEmulator_(bufferedQueue_, emulatedLatency, stdDevMeanLatency)
, transceiver_(latencyEmulator_)
, masterPeerEndpoint_(boost::asio::ip::address::from_string(address), port) {
    currentState.reset(new GamePeer::Connecting{this});
}

void GamePeer::update(const Clock& clock) {
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

void GamePeer::handleEvent(SDL_Event &event, bool& running) {
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

void GamePeer::setState(StatePtr& newState) {
    nextState = newState;
}

void GamePeer::processIncomingPackets(const Clock& clock) {
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

void GamePeer::renderFrame() {
    renderer_.clear(0.25f, 0.25f, 0.25f);
    world_.draw(renderer_);
    renderer_.present();
}

GamePeer::State::State(GamePeer* gamePeer)
: gamePeer_(gamePeer) {
}

GamePeer::Peering::Peering(GamePeer* gamePeer)
: State(gamePeer)
, lastTickTime_(0) {
}

void GamePeer::Peering::handleIncomingPacket(Packet* packet, const Clock& clock) {
    unsigned char packetType = PROTOCOL_PACKET_TYPE_INVALID;
    packet->read(packetType);
    switch (packetType) {
    case PROTOCOL_PACKET_TYPE_TICK:
        handleTick(packet, clock);
        break;
    case PROTOCOL_PACKET_TYPE_TOCK:
        handleTock(packet, clock);
        break;
    default:
        handleIncomingPacketType(packetType, packet, clock);
        break;
    }
}

void GamePeer::Peering::handleTick(Packet* packet, const Clock&) {
    uint32_t playerId = PROTOCOL_INVALID_PLAYER_ID;
    packet->read(playerId);
    if (gamePeer_->peerRegistry_.verifyPeer(playerId, packet->getEndpoint())) {
        // TODO: Store time

        float timeStamp = 0.0f;
        packet->read(timeStamp);

        auto replyPacket = gamePeer_->bufferedQueue_.pop();
        if (replyPacket) {
            createTockPacket(replyPacket, timeStamp, packet->getEndpoint());
            gamePeer_->transceiver_.sendTo(replyPacket);
        } else {
            WARN("Failed to send TOCK to peer {0}: empty packet pool.", packet->getEndpoint());
        }
    } else {
        WARN("Received INPUT from unknown peer {0}.", packet->getEndpoint());
    }
}

void GamePeer::Peering::handleTock(Packet* packet, const Clock& clock) {
    float timeStamp = 0.0f;
    packet->read(timeStamp);
    const float roundTripTime = clock.getTime() - timeStamp;
    gamePeer_->peerRegistry_.addRoundTripTime(packet->getEndpoint(), roundTripTime);
}

void GamePeer::Peering::sendOutgoingPackets(const Clock& clock) {
    const auto now = clock.getTime();
    if (now > lastTickTime_ + 1.0f) {
        sendTick(clock);
        lastTickTime_ = now;
    }
}

void GamePeer::Peering::sendTick(const Clock& clock) {
    gamePeer_->peerRegistry_.forEachPeer([this, &clock] (const Peer& peer) {
        auto packet = gamePeer_->bufferedQueue_.pop();
        if (packet) {
            createTickPacket(packet, gamePeer_->playerId_, clock.getTime(), peer.endpoint);
            gamePeer_->transceiver_.sendTo(packet);
        } else {
            WARN("Failed to send TICK to server: empty packet pool.");
        }
    });
}

GamePeer::Accepting::Accepting(GamePeer* gamePeer)
: Peering(gamePeer)
, nextPlayerId_(gamePeer_->playerId_ + 1) {
}

void GamePeer::Accepting::handleIncomingPacketType(unsigned char packetType, Packet* packet, const Clock&) {
    switch (packetType) {
    case PROTOCOL_PACKET_TYPE_HELLO:
        handleHello(packet);
        break;
    default:
        WARN("Received a packet with unexpected packet type {0} from {1}.", static_cast<unsigned int>(packetType), packet->getEndpoint());
        break;
    }
}

void GamePeer::Accepting::handleHello(Packet* packet) {
    if (gamePeer_->peerRegistry_.getCount() < PROTOCOL_NUM_PEERS_FOR_GAME) {
        if (!gamePeer_->peerRegistry_.isRegistered(packet->getEndpoint())) {
            INFO("HELLO received from new peer at {0}", packet->getEndpoint());
            auto invitePacket = gamePeer_->bufferedQueue_.pop();
            if (invitePacket) {
                const auto playerId = nextPlayerId_++;

                createInvitePacket(invitePacket, playerId, packet->getEndpoint(), gamePeer_->peerRegistry_);
                gamePeer_->transceiver_.sendTo(invitePacket);
                INFO("Sending INVITE to peer at {0}.", packet->getEndpoint());

                gamePeer_->peerRegistry_.forEachPeer([this, packet, playerId] (const Peer& peer) {
                    auto introPacket = gamePeer_->bufferedQueue_.pop();
                    if (introPacket) {
                        createIntroPacket(introPacket, playerId, packet->getEndpoint(), peer.endpoint);
                        gamePeer_->transceiver_.sendTo(introPacket);
                        INFO("Sending INTRO to peer at {0}.", peer.endpoint);
                    } else {
                        WARN("Failed to send INTRO to peer at {0}: empty packet pool", peer.endpoint);
                    }
                });

                gamePeer_->peerRegistry_.add(packet->getEndpoint(), playerId);
            } else {
                WARN("Failed to send INVITE to new peer: empty packet pool.");
            }
        } else {
            WARN("HELLO from a registered peer {0}.", packet->getEndpoint());
        }
    } else {
        WARN("HELLO received from {0} but number of necessary players already reached.", packet->getEndpoint());
    }
}

void GamePeer::Accepting::sendOutgoingPackets(const Clock& clock) {
    Peering::sendOutgoingPackets(clock);

    if (gamePeer_->peerRegistry_.getCount() == PROTOCOL_NUM_PEERS_FOR_GAME) {
        gamePeer_->peerRegistry_.forEachPeer([this] (const Peer& peer) {
            auto startPacket = gamePeer_->bufferedQueue_.pop();
            if (startPacket) {
                createStartPacket(startPacket, peer.endpoint);
                gamePeer_->transceiver_.sendTo(startPacket);
                INFO("Sending START to peer at {0}.", peer.endpoint);
            } else {
                WARN("Failed to send START to peer at {0}: empty packet pool", peer.endpoint);
            }
        });
        auto newState = StatePtr(new Playing{gamePeer_});
        gamePeer_->setState(newState);
    }
}

GamePeer::Connecting::Connecting(GamePeer* gamePeer)
: State(gamePeer)
, lastHelloTime_(0) {
}

void GamePeer::Connecting::handleIncomingPacket(Packet* packet, const Clock&) {
    unsigned char packetType = PROTOCOL_PACKET_TYPE_INVALID;
    packet->read(packetType);
    switch (packetType) {
    case PROTOCOL_PACKET_TYPE_INVITE:
        handleInvite(packet);
        break;
    default:
        WARN("Received a packet with unexpected packet type {0} from {1}.", static_cast<unsigned int>(packetType), packet->getEndpoint());
        break;
    }
}

void GamePeer::Connecting::handleInvite(Packet* packet) {
    packet->read(gamePeer_->playerId_);

    DEBUG("INVITE received from master peer at {0}. My player ID is {1}.", packet->getEndpoint(), gamePeer_->playerId_);

    gamePeer_->peerRegistry_.add(packet->getEndpoint(), 1);

    int32_t peerCount = 0;
    packet->read(peerCount);
    for (int32_t i = 0; i < peerCount; ++i) {
        std::string address;
        packet->read(address);
        unsigned short port = 0;
        packet->read(port);
        uint32_t playerId = 0;
        packet->read(playerId);
        const boost::asio::ip::udp::endpoint peerEndpoint(boost::asio::ip::address::from_string(address), port);
        gamePeer_->peerRegistry_.add(peerEndpoint, playerId);
        DEBUG("Adding peer at {0}:{1}", address, port);
    }

    auto newState = StatePtr(new Waiting{gamePeer_});
    gamePeer_->setState(newState);
}

void GamePeer::Connecting::sendOutgoingPackets(const Clock& clock) {
    sendHello(clock);
}

void GamePeer::Connecting::sendHello(const Clock& clock) {
    const auto now = clock.getTime();
    if (now > lastHelloTime_ + PROTOCOL_HELLO_INTERVAL) {
        auto packet = gamePeer_->bufferedQueue_.pop();
        if (packet) {
            createHelloPacket(packet, gamePeer_->masterPeerEndpoint_);
            INFO("Send HELLO to {0}.", packet->getEndpoint());
            gamePeer_->transceiver_.sendTo(packet);
        } else {
            WARN("Failed to send HELLO to server: empty packet pool.");
        }
        lastHelloTime_ = now;
    }
}

GamePeer::Waiting::Waiting(GamePeer* gamePeer)
: Peering(gamePeer) {
}

void GamePeer::Waiting::handleIncomingPacketType(unsigned char packetType, Packet* packet, const Clock&) {
    switch (packetType) {
    case PROTOCOL_PACKET_TYPE_INTRO:
        handleIntro(packet);
        break;
    case PROTOCOL_PACKET_TYPE_START:
        handleStart(packet);
        break;
    default:
        WARN("Received a packet with unexpected packet type {0} from {1}.", static_cast<unsigned int>(packetType), packet->getEndpoint());
        break;
    }
}

void GamePeer::Waiting::handleIntro(Packet* packet) {
    uint32_t playerId = 0;
    packet->read(playerId);
    DEBUG("INTRO received from master peer at {0}. New peers player ID is {1}.", packet->getEndpoint(), playerId);
    std::string address;
    packet->read(address);
    unsigned short port = 0;
    packet->read(port);
    const boost::asio::ip::udp::endpoint peerEndpoint(boost::asio::ip::address::from_string(address), port);
    gamePeer_->peerRegistry_.add(peerEndpoint, playerId);
    DEBUG("Adding peer at {0}:{1}", address, port);
}

void GamePeer::Waiting::handleStart(Packet*) {
    auto newState = StatePtr(new Playing{gamePeer_});
    gamePeer_->setState(newState);
}

void GamePeer::Waiting::sendOutgoingPackets(const Clock& clock) {
    Peering::sendOutgoingPackets(clock);
}

GamePeer::Playing::Playing(GamePeer* gamePeer)
: Peering(gamePeer)
, nextObjectId_(gamePeer_->playerId_ * 1000)
, lastStateUpdate_(0) {
    INFO("Playing");
}

void GamePeer::Playing::handleWillUpdateWorld(const Clock& clock) {
    static bool initialized_ = false;

    gamePeer_->inputHandler_.update(clock.getGameTime());

    if (!initialized_) {
        auto gameObjectPtr = GameObjectPtr(new LocalSpaceShip(gamePeer_->renderer_, gamePeer_->inputHandler_,
            [this, &clock] (SpaceShip* spaceShip, float lastShot) -> float {
                const auto now = clock.getTime();
                if (now > lastShot + 0.5f) {
                    auto laserBolt = GameObjectPtr(new LaserBolt(gamePeer_->renderer_, spaceShip->getPosition(), 50.0f * spaceShip->getLookAt()));
                    gamePeer_->world_.addLocalGameObject(nextObjectId_++, laserBolt);
                    return now;
                }
                return lastShot;
            }));
        gamePeer_->world_.addLocalGameObject(nextObjectId_++, gameObjectPtr);
        initialized_ = true;
    }
}

void GamePeer::Playing::handleIncomingPacketType(unsigned char packetType, Packet* packet, const Clock&) {
    switch (packetType) {
    case PROTOCOL_PACKET_TYPE_STATE:
        handleState(packet);
        break;
    default:
        WARN("Received a packet with unexpected packet type {0} from {1}.", static_cast<unsigned int>(packetType), packet->getEndpoint());
        break;
    }
}

void GamePeer::Playing::handleState(Packet* packet) {
    float latestInputTime = 0.0f;
    packet->read(latestInputTime);
    assert(latestInputTime == 0.0f);

    std::set<uint32_t> gameObjectsToRemove;
    gamePeer_->world_.forEachRemoteGameObject(packet->getEndpoint(), [&gameObjectsToRemove] (uint32_t objectId, GameObject*) {
        gameObjectsToRemove.insert(objectId);
    });

    uint32_t gameObjectCount = 0;
    packet->read(gameObjectCount);
    for (uint32_t i = 0; i < gameObjectCount; i++) {
        uint32_t objectId = 0, classId = 0;
        packet->read(objectId);
        gameObjectsToRemove.erase(objectId);
        packet->read(classId);
        auto gameObject = gamePeer_->world_.getRemoteGameObject(objectId);
        if (gameObject == nullptr) {
            const auto& latencyEstimator = gamePeer_->peerRegistry_.getLatencyEstimator(packet->getEndpoint());
            GameObjectPtr gameObjectPtr;
            if (classId == SpaceShip::ClassId) {
                gameObjectPtr = GameObjectPtr(new RemoteSpaceShip(gamePeer_->renderer_, latencyEstimator, gamePeer_->frameDuration_));
            } else if (classId == LaserBolt::ClassId) {
                gameObjectPtr = GameObjectPtr(new RemoteLaserBolt(gamePeer_->renderer_, latencyEstimator, gamePeer_->frameDuration_));
            }
            gamePeer_->world_.addRemoteGameObject(objectId, gameObjectPtr, packet->getEndpoint());
            gameObject = gameObjectPtr.get();
        }
        gameObject->read(packet);
    }

    for (const auto objectId : gameObjectsToRemove) {
        gamePeer_->world_.removeRemoteGameObject(objectId);
    }
}

void GamePeer::Playing::sendOutgoingPackets(const Clock& clock) {
    Peering::sendOutgoingPackets(clock);

    const auto now = clock.getTime();
    if (now > lastStateUpdate_ + gamePeer_->updateInterval_) {
        gamePeer_->peerRegistry_.forEachPeer([this] (const Peer& peer) {
            auto packet = gamePeer_->bufferedQueue_.pop();
            if (packet) {
                createStatePacket(packet, peer.endpoint);
                packet->write(0.0f);
                packet->write(gamePeer_->world_.getLocalGameObjectCount());
                gamePeer_->world_.forEachLocalGameObject([this, packet] (uint32_t objectId, GameObject* gameObject) {
                    uint32_t playerId = objectId / 1000;
                    if (playerId == gamePeer_->playerId_) {
                        packet->write(objectId);
                        packet->write(gameObject->getClassId());
                        gameObject->write(packet);
                    }
                });
                gamePeer_->transceiver_.sendTo(packet);
            } else {
                WARN("Failed to send STATE to peer at {0}: empty packet pool", peer.endpoint);
            }
        });
        lastStateUpdate_ = now;
    }
}
