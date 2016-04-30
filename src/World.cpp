#include "World.h"

#include <cassert>

World::World() {
}

void World::add(uint32_t objectId, GameObjectPtr& gameObject) {
    assert(gameObjects_.find(objectId) == gameObjects_.end());
    gameObjects_.insert(ObjectIdToGameObjectMap::value_type(objectId, gameObject));
}

void World::remove(uint32_t objectId) {
    gameObjects_.erase(gameObjects_.find(objectId));
}

GameObject* World::getGameObject(uint32_t objectId) {
    auto itr = gameObjects_.find(objectId);
    if (itr != gameObjects_.end()) {
        return itr->second.get();
    }
    return nullptr;
}

uint32_t World::getGameObjectCount() const {
    return static_cast<uint32_t>(gameObjects_.size());
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

void World::forEachGameObject(std::function<void (uint32_t, GameObject*)> fun) {
    for (auto& gameObject : gameObjects_) {
        fun(gameObject.first, gameObject.second.get());
    }
}

// void World::write(Packet* packet) {
//     packet->write(gameObjects_.size());
//     for (const auto& gameObject : gameObjects_) {
//         packet->write(gameObject.first);
//         gameObject.second->write(packet);
//     }
// }

// void World::read(Packet* packet) {
//     std::size_t stateCount = 0;
//     packet->read(stateCount);
//     for (std::size_t i = 0; i < stateCount; i++) {
//         uint32_t objectId = PROTOCOL_INVALID_OBJECT_ID;
//         packet->read(objectId);

//         // Check if we have this object already.

//         uint32_t classId = PROTOCOL_INVALID_CLASS_ID;
//         packet->read(classId);

//         GameObjectPtr gameObject;

//         auto itr = gameObjects_.find(objectId);
//         if (itr == gameObjects_.end()) {

//         } else {
//             gameObject = itr->second;
//         }

//         gameObject->read(packet);
//     }
// }
