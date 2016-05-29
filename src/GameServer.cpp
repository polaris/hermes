#include "GameServer.h"
#include "Renderer.h"
#include "Clock.h"
#include "Protocol.h"
#include "ServerSpaceShip.h"
#include "LaserBolt.h"
#include "ClientSession.h"
#include "Packet.h"
#include "Utilities.h"
#include "Logging.h"

GameServer::GameServer(unsigned int width, unsigned int height, unsigned int frameRate, unsigned int updateRate, unsigned emulatedLatency, uint16_t port, Renderer& renderer)
: Game(frameRate, renderer)
, width_(width)
, height_(height)
, world_(width, height, [this] (uint32_t objectId1, uint32_t objectId2) { return confirmCollision(objectId1, objectId2); }, [this] (uint32_t objectId) { removedObject(objectId); })
, updateInterval_(1.0f/static_cast<float>(updateRate))
, nextPlayerId_(1)
, nextObjectId_(1)
, playerToObjectMap_()
, objectToPlayerMap_()
, bufferedQueue_(4000)
, latencyEmulator_(bufferedQueue_, emulatedLatency)
, transceiver_(port, latencyEmulator_)
, clientRegistry_()
, lastStateUpdate_(0) {
}

void GameServer::update(const Clock& clock) {
    processIncomingPackets(clock);
    checkForDisconnects(clock);
    updateWorld();
    renderWorld();
    sendOutgoingPackets(clock);
}

void GameServer::processIncomingPackets(const Clock& clock) {
    auto packet = bufferedQueue_.dequeue();
    if (packet) {
        uint32_t magicNumber = 0;
        packet->read(magicNumber);
        if (magicNumber == PROTOCOL_MAGIC_NUMBER) {
            handlePacket(packet, clock);
        } else {
            WARN("Received an invalid packet from {0}: wrong magic number.", packet->getEndpoint());
        }
        bufferedQueue_.push(packet);
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
        case PROTOCOL_PACKET_TYPE_TICK:
            handleTick(packet, clock);
            break;
        default:
            WARN("Received a packet with unexpected packet type {0} from {1}.", static_cast<unsigned int>(packetType), packet->getEndpoint());
            break;
        }
    } else {
        WARN("Received a packet with invalid protocol version {0} from {1}.", static_cast<unsigned int>(protocolVersion), packet->getEndpoint());
    }
}

void GameServer::handleHello(Packet* packet, const Clock& clock) {
    if (!clientRegistry_.hasClientSession(packet->getEndpoint())) {
        INFO("HELLO received from new client at {0}", packet->getEndpoint());
        auto welcomePacket = bufferedQueue_.pop();
        if (welcomePacket) {
            const auto playerId = nextPlayerId_++;
            const auto objectId = nextObjectId_++;

            ClientSession* clientSession = clientRegistry_.addClientSession(playerId, packet->getEndpoint(), clock.getTime());

            SpaceShipPtr newSpaceShip(new ServerSpaceShip(renderer_, Vector2d(randomValue(width_), randomValue(height_)), clientSession, [this, &clock, playerId] (SpaceShip* spaceShip, float lastShot) -> float {
                const auto now = clock.getTime();
                if (now > lastShot + 0.25f) {
                    auto laserBolt = GameObjectPtr(new LaserBolt(renderer_, spaceShip->getPosition() + 5.0f * spaceShip->getLookAt(), 100.0f * spaceShip->getLookAt()));
                    const auto boldObjectId = nextObjectId_++;
                    world_.add(boldObjectId, laserBolt);
                    objectToPlayerMap_[boldObjectId] = playerId;
                    return now;
                }
                return lastShot;
            }));
            newSpaceShip->setPlayerId(playerId);

            auto gameObject = std::dynamic_pointer_cast<GameObject>(newSpaceShip);
            world_.add(objectId, gameObject);

            playerToObjectMap_[playerId] = objectId;
            objectToPlayerMap_[objectId] = playerId;

            createWelcomePacket(welcomePacket, playerId, objectId, packet->getEndpoint());
            transceiver_.sendTo(welcomePacket);

            INFO("WELCOME client {0} from {1}.", playerId, packet->getEndpoint());
        } else {
            WARN("Failed to WELCOME client: empty packet pool.");
        }
    } else {
        WARN("HELLO from known client {0}.", packet->getEndpoint());
    }
}

void GameServer::handleInput(Packet* packet, const Clock& clock) {
    uint32_t playerId = PROTOCOL_INVALID_PLAYER_ID;
    packet->read(playerId);
    if (clientRegistry_.verifyClientSession(playerId, packet->getEndpoint())) {
        auto clientSession = clientRegistry_.getClientSession(playerId);
        clientSession->setLastSeen(clock.getTime());
        MoveList& moveList = clientSession->getMoveList();
        float timeStamp = 0.0f;
        packet->read(timeStamp);
        clientSession->setLatestInputTime(timeStamp);
        uint32_t count = 0;
        packet->read(count);
        for (uint32_t i = 0; i < count; i++) {
            Move move;
            move.read(packet);
            moveList.addMove(move);
        }
    } else {
        WARN("Received INPUT from unknown client {0}.", packet->getEndpoint());
    }
}

void GameServer::handleTick(Packet* packet, const Clock& clock) {
    uint32_t playerId = PROTOCOL_INVALID_PLAYER_ID;
    packet->read(playerId);
    if (clientRegistry_.verifyClientSession(playerId, packet->getEndpoint())) {
        auto clientSession = clientRegistry_.getClientSession(playerId);
        clientSession->setLastSeen(clock.getTime());
        float timeStamp = 0.0f;
        packet->read(timeStamp);

        auto replyPacket = bufferedQueue_.pop();
        if (replyPacket) {
            createTockPacket(replyPacket, timeStamp, clientSession->getEndpoint());
            transceiver_.sendTo(replyPacket);
        } else {
            WARN("Failed to send TOCK to a client: empty packet pool.");
        }

    } else {
        WARN("Received INPUT from unknown client {0}.", packet->getEndpoint());
    }
}

void GameServer::checkForDisconnects(const Clock& clock) {
    clientRegistry_.checkForDisconnects(clock.getTime(), [this] (uint32_t playerId) {
            DEBUG("Remove disconnected client {0}", playerId);
            auto itr = playerToObjectMap_.find(playerId);
            if (itr != playerToObjectMap_.end()) {
                const uint32_t objectId = itr->second;
                world_.remove(objectId);
                removedObject(objectId);
                playerToObjectMap_.erase(itr);
            }
        });
}

void GameServer::updateWorld() {
    world_.update(frameDuration_);
}

void GameServer::renderWorld() {
    renderer_.clear(0.25f, 0.25f, 0.25f);
    world_.draw(renderer_);
    renderer_.present();
}

void GameServer::sendOutgoingPackets(const Clock& clock) {
    sendStateUpdate(clock);
}

void GameServer::sendStateUpdate(const Clock& clock) {
    const auto now = clock.getTime();
    if (now > lastStateUpdate_ + updateInterval_) {
        clientRegistry_.forEachClientSession(
            [this] (ClientSession* clientSession) {
                auto packet = bufferedQueue_.pop();
                if (packet) {
                    createStatePacket(packet, clientSession->getEndpoint());
                    packet->write(clientSession->getLatestInputTime());
                    packet->write(world_.getGameObjectCount());
                    world_.forEachGameObject([this, packet] (uint32_t objectId, GameObject* gameObject) {
                        packet->write(objectId);
                        packet->write(gameObject->getClassId());
                        gameObject->write(packet);
                    });

                    transceiver_.sendTo(packet);
                } else {
                    WARN("Failed send STATE update to a client: empty packet pool.");
                }
            });

        lastStateUpdate_ = now;
    }
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

bool GameServer::confirmCollision(uint32_t objectId1, uint32_t objectId2) {
    return objectToPlayerMap_[objectId1] != objectToPlayerMap_[objectId2];
}

void GameServer::removedObject(uint32_t objectId) {
    const auto itr = objectToPlayerMap_.find(objectId);
    if (itr != objectToPlayerMap_.end()) {
        objectToPlayerMap_.erase(objectId);
    }

}
