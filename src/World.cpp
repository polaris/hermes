#include "World.h"
#include "Packet.h"
#include "Protocol.h"

#include <cassert>

World::World() {
}

void World::add(unsigned int objectId, GameObjectPtr& gameObject) {
    assert(gameObjects_.find(objectId) == gameObjects_.end());
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
        packet->write(gameObject.first);
        gameObject.second->write(packet);
    }
}

void World::read(Packet* packet) {
    std::size_t stateCount = 0;
    packet->read(stateCount);
    for (std::size_t i = 0; i < stateCount; i++) {
        unsigned int objectId = PROTOCOL_INVALID_OBJECT_ID;
        packet->read(objectId);

        // Check if we have this object already.

        unsigned int classId = PROTOCOL_INVALID_CLASS_ID;
        packet->read(classId);

        GameObjectPtr gameObject;

        auto itr = gameObjects_.find(objectId);
        if (itr == gameObjects_.end()) {

        } else {
            gameObject = itr->second;
        }

        gameObject->read(packet);
    }
}
