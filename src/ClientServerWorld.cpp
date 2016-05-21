#include "ClientServerWorld.h"

#include <cassert>

ClientServerWorld::ClientServerWorld()
: gameObjects_() {
}

void ClientServerWorld::add(uint32_t objectId, GameObjectPtr& gameObject) {
    assert(gameObjects_.find(objectId) == gameObjects_.end());
    gameObjects_.insert(ObjectIdToGameObjectMap::value_type(objectId, gameObject));
}

void ClientServerWorld::remove(uint32_t objectId) {
    gameObjects_.erase(gameObjects_.find(objectId));
}

GameObject* ClientServerWorld::getGameObject(uint32_t objectId) {
    auto itr = gameObjects_.find(objectId);
    if (itr != gameObjects_.end()) {
        return itr->second.get();
    }
    return nullptr;
}

uint32_t ClientServerWorld::getGameObjectCount() const {
    return static_cast<uint32_t>(gameObjects_.size());
}

void ClientServerWorld::update(float elapsed) {
    for (auto& gameObject : gameObjects_) {
        gameObject.second->update(elapsed);
    }
}

void ClientServerWorld::draw(Renderer& renderer) {
    for (auto& gameObject : gameObjects_) {
        gameObject.second->draw(renderer);
    }
}

void ClientServerWorld::forEachGameObject(std::function<void (uint32_t, GameObject*)> fun) {
    for (auto& gameObject : gameObjects_) {
        fun(gameObject.first, gameObject.second.get());
    }
}
