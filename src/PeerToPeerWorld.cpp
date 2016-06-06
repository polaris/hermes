#include "PeerToPeerWorld.h"
#include "Vector2d.h"
#include "SpaceShip.h"

#include <unordered_set>
#include <cassert>

PeerToPeerWorld::PeerToPeerWorld(unsigned int width, unsigned int height, ConfirmCollisionFunc confirmCollisionFunc)
: width_(width)
, height_(height)
, confirmCollisionFunc_(confirmCollisionFunc)
, localGameObjects_()
, remoteGameObjects_()
, remoteEndpoints_() {
}

void PeerToPeerWorld::addLocalGameObject(uint32_t objectId, GameObjectPtr& gameObject) {
    assert(localGameObjects_.find(objectId) == localGameObjects_.end());
    localGameObjects_.insert(ObjectIdToGameObjectMap::value_type(objectId, gameObject));
}

void PeerToPeerWorld::removeLocalGameObject(uint32_t objectId) {
    localGameObjects_.erase(localGameObjects_.find(objectId));
}

void PeerToPeerWorld::addRemoteGameObject(uint32_t objectId, GameObjectPtr& gameObject, const boost::asio::ip::udp::endpoint& endpoint) {
    assert(remoteGameObjects_.find(objectId) == remoteGameObjects_.end());
    remoteGameObjects_.insert(ObjectIdToGameObjectMap::value_type(objectId, gameObject));
    remoteEndpoints_.insert(ObjectIdToEndpointMap::value_type(objectId, endpoint));
}

void PeerToPeerWorld::removeRemoteGameObject(uint32_t objectId) {
    remoteGameObjects_.erase(remoteGameObjects_.find(objectId));
    remoteEndpoints_.erase(remoteEndpoints_.find(objectId));
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

    forEachLocalGameObject([this] (uint32_t objectId1, GameObject* gameObject1) {
        const auto& pos = gameObject1->getPosition();
        if ((pos.x() < 0 || pos.y() < 0 || pos.x() > width_ || pos.y() > height_) && gameObject1->getClassId() != SpaceShip::ClassId) {
            gameObject1->kill();
        }
        if (gameObject1->doesCollide()) {
            forEachRemoteGameObject([this, objectId1, gameObject1] (uint32_t objectId2, GameObject* gameObject2) {
                if (gameObject2->doesCollide() && gameObject1->checkCollision(gameObject2) && confirmCollisionFunc_(objectId1, gameObject1, objectId2, gameObject2)) {
                    gameObject1->kill();
                }
            });
        }
    });
    removeLocalGameObjectIf([this] (uint32_t, GameObject* gameObject) {
        return gameObject->dead();
    });
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

void PeerToPeerWorld::forEachRemoteGameObject(std::function<void (uint32_t, GameObject*)> fun) {
    for (auto& gameObject : remoteGameObjects_) {
        fun(gameObject.first, gameObject.second.get());
    }
}

void PeerToPeerWorld::forEachRemoteGameObject(const boost::asio::ip::udp::endpoint& endpoint, std::function<void (uint32_t, GameObject*)> fun) {
    for (auto& gameObject : remoteGameObjects_) {
        if (remoteEndpoints_[gameObject.first] == endpoint) {
            fun(gameObject.first, gameObject.second.get());
        }
    }
}

void PeerToPeerWorld::removeLocalGameObjectIf(std::function<bool (uint32_t, GameObject*)> predicate) {
    for(auto itr = begin(localGameObjects_); itr != end(localGameObjects_);) {
        if (predicate(itr->first, itr->second.get())) {
            itr = localGameObjects_.erase(itr);
        } else {
            ++itr;
        }
    }
}
