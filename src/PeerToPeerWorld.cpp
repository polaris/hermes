#include "PeerToPeerWorld.h"

#include <cassert>

PeerToPeerWorld::PeerToPeerWorld()
: localGameObjects_()
, remoteGameObjects_() {
}

void PeerToPeerWorld::addLocalGameObject(uint32_t objectId, GameObjectPtr& gameObject) {
    assert(localGameObjects_.find(objectId) == localGameObjects_.end());
    localGameObjects_.insert(ObjectIdToGameObjectMap::value_type(objectId, gameObject));
}

void PeerToPeerWorld::removeLocalGameObject(uint32_t objectId) {
    localGameObjects_.erase(localGameObjects_.find(objectId));
}

void PeerToPeerWorld::addRemoteGameObject(uint32_t objectId, GameObjectPtr& gameObject) {
    assert(remoteGameObjects_.find(objectId) == remoteGameObjects_.end());
    remoteGameObjects_.insert(ObjectIdToGameObjectMap::value_type(objectId, gameObject));
}

void PeerToPeerWorld::removeRemoteGameObject(uint32_t objectId) {
    remoteGameObjects_.erase(remoteGameObjects_.find(objectId));
}

GameObject* PeerToPeerWorld::getRemoteGameObject(uint32_t objectId) {
    auto itr = remoteGameObjects_.find(objectId);
    if (itr != remoteGameObjects_.end()) {
        return itr->second.get();
    }
    return nullptr;
}

uint32_t PeerToPeerWorld::getLocalGameObjectCount() const {
    return static_cast<uint32_t>(localGameObjects_.size());
}

void PeerToPeerWorld::update(float elapsed) {
    for (auto& gameObject : localGameObjects_) {
        gameObject.second->update(elapsed);
    }
    for (auto& gameObject : remoteGameObjects_) {
        gameObject.second->update(elapsed);
    }
}

void PeerToPeerWorld::draw(Renderer& renderer) {
    for (auto& gameObject : localGameObjects_) {
        gameObject.second->draw(renderer);
    }
    for (auto& gameObject : remoteGameObjects_) {
        gameObject.second->draw(renderer);
    }
}

void PeerToPeerWorld::forEachLocalGameObject(std::function<void (uint32_t, GameObject*)> fun) {
    for (auto& gameObject : localGameObjects_) {
        fun(gameObject.first, gameObject.second.get());
    }
}
