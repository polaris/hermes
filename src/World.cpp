#include "World.h"

#include <cassert>

World::World()
: gameObjects_() {
}

void World::add(uint32_t objectId, GameObjectPtr& gameObject) {
    assert(gameObjects_.find(objectId) == gameObjects_.end());
    gameObjects_.insert(ObjectIdToGameObjectMap::value_type(objectId, gameObject));
}

void World::remove(uint32_t objectId) {
    const auto itr = gameObjects_.find(objectId);
    if (itr != gameObjects_.end()) {
        gameObjects_.erase(itr);
    }
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
