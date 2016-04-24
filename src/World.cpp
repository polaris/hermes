#include "World.h"
#include "Packet.h"
#include "Protocol.h"

World::World() {
}

void World::add(unsigned int objectId, GameObjectPtr& gameObject) {
    gameObjects_.insert(ObjectIdToGameObjectMap::value_type(objectId, gameObject));
}

void World::remove(unsigned int objectId) {
    gameObjects_.erase(gameObjects_.find(objectId));
}

void World::update(float elapsed) {
    for (auto& gameObject : gameObjects_) {
        gameObject.second->update(elapsed);
    }
}

void World::draw(Renderer& renderer) {
    for (auto& gameObject : gameObjects_) {
        gameObject.second->draw(renderer);
    }
}

void World::write(Packet* packet) {
    packet->write(gameObjects_.size());
    for (const auto& gameObject : gameObjects_) {
        gameObject.second->write(packet);
    }
}

void World::read(Packet* packet) {
    std::size_t stateCount = 0;
    packet->read(stateCount);
    for (std::size_t i = 0; i < stateCount; i++) {
        unsigned int playerId = PROTOCOL_INVALID_PLAYER_ID;
        packet->read(playerId);
        unsigned int objectId = PROTOCOL_INVALID_OBJECT_ID;
        packet->read(objectId);

        GameObjectPtr gameObject;

        auto itr = gameObjects_.find(objectId);
        if (itr == gameObjects_.end()) {

        } else {
            gameObject = itr->second;
        }

        gameObject->read(packet);
    }
}
