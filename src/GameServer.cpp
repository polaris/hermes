#include "GameServer.h"
#include "Renderer.h"
#include "Clock.h"
#include "Protocol.h"
#include "ServerSpaceShip.h"
#include "ClientSession.h"
#include "Packet.h"
#include "Logging.h"

GameServer::GameServer(unsigned int frameRate, unsigned int updateRate, uint16_t port, Renderer& renderer)
: Game(frameRate, renderer)
, updateInterval_(1.0f/static_cast<float>(updateRate))
, nextPlayerId_(1)
, nextObjectId_(1)
, playerToObjectMap_()
, newClients_()
, bufferedQueue_(4000)
, latencyEmulator_(bufferedQueue_, 150)
, transceiver_(port, latencyEmulator_)
, clientRegistry_()
, lastStateUpdate_(0) {
}

void GameServer::handleWillUpdateWorld(const Clock& clock) {
    processIncomingPackets(clock);
    clientRegistry_.checkForDisconnects(clock.getTime(), [this] (uint32_t playerId) {
        DEBUG("Remove disconnected client {0}", playerId);
        auto itr = playerToObjectMap_.find(playerId);
        if (itr != playerToObjectMap_.end()) {
            const uint32_t objectId = itr->second;
            world_.remove(objectId);
            playerToObjectMap_.erase(itr);
        }
    });
}

void GameServer::processIncomingPackets(const Clock& clock) {
    auto packet = bufferedQueue_.dequeue();
    if (packet) {
        uint32_t magicNumber = 0;
        packet->read(magicNumber);
        if (magicNumber == PROTOCOL_MAGIC_NUMBER) {
            handlePacket(packet, clock);
        } else {
            WARN("Receive invalid packet from {0}", packet->getEndpoint());
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
        spdlog::get("console")->debug("HELLO from new client ...");
        auto welcomePacket = bufferedQueue_.pop();
        if (welcomePacket) {
            const auto playerId = nextPlayerId_++;
            const auto objectId = nextObjectId_++;

            ClientSession* clientSession = clientRegistry_.addClientSession(playerId, packet->getEndpoint(), clock.getTime());

            SpaceShipPtr newSpaceShip(new ServerSpaceShip(renderer_, clientSession));
            newSpaceShip->setPlayerId(playerId);

            auto gameObject = std::dynamic_pointer_cast<GameObject>(newSpaceShip);
            world_.add(objectId, gameObject);
            playerToObjectMap_[playerId] = objectId;

            newClients_.push_back(playerId);

            createWelcomePacket(welcomePacket, playerId, objectId, packet->getEndpoint());
            transceiver_.sendTo(welcomePacket);

            INFO("WELCOME client {0} from {1}.", playerId, packet->getEndpoint());
        } else {
            WARN("Failed to WELCOME to client: empty packet pool.");
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

void GameServer::handleDidUpdateWorld(const Clock& clock) {
    renderWorld();
    sendOutgoingPackets(clock);
}

void GameServer::renderWorld() {
    renderer_.clear(0.25f, 0.25f, 0.25f);
    world_.draw(renderer_);
    renderer_.present();
}

void GameServer::sendOutgoingPackets(const Clock& clock) {
    sendStateToNewClients();
    sendStateUpdate(clock);
}

void GameServer::sendStateToNewClients() {
    for (const auto& playerId : newClients_) {
        auto packet = bufferedQueue_.pop();
        if (packet) {
            const auto clientSession = clientRegistry_.getClientSession(playerId);
            createStatePacket(packet, clientSession->getEndpoint());

            packet->write(world_.getGameObjectCount());
            world_.forEachGameObject([this, packet] (uint32_t objectId, GameObject* gameObject) {
                packet->write(objectId);
                packet->write(gameObject->getClassId());
                gameObject->write(packet);
            });

            transceiver_.sendTo(packet);
        } else {
            WARN("Failed to send STATE to a client: empty packet pool.");
        }
    }
    newClients_.clear();
}

void GameServer::sendStateUpdate(const Clock& clock) {
    const auto now = clock.getTime();
    if (now > lastStateUpdate_ + updateInterval_) {
        uint32_t dirtyGameObjectCount = 0;
        world_.forEachGameObject([&dirtyGameObjectCount] (uint32_t, GameObject* gameObject) {
            if (gameObject->isDirty()) {
                dirtyGameObjectCount += 1;
            }
        });

        if (dirtyGameObjectCount > 0) {
            clientRegistry_.forEachClientSession([this, dirtyGameObjectCount] (ClientSession* clientSession) {
                    auto packet = bufferedQueue_.pop();
                    if (packet) {
                        createStatePacket(packet, clientSession->getEndpoint());

                        packet->write(dirtyGameObjectCount);
                        world_.forEachGameObject([this, packet] (uint32_t objectId, GameObject* gameObject) {
                            if (gameObject->isDirty()) {
                                packet->write(objectId);
                                packet->write(gameObject->getClassId());
                                gameObject->write(packet);
                            }
                        });

                        transceiver_.sendTo(packet);
                    } else {
                        WARN("Failed send STATE update to a client: empty packet pool.");
                    }
                });
        }

        world_.forEachGameObject([] (uint32_t, GameObject* gameObject) {
            gameObject->resetDirty();
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
