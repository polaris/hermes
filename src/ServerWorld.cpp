#include "ServerWorld.h"
#include "Vector2d.h"
#include "SpaceShip.h"

#include <set>

ServerWorld::ServerWorld(unsigned int width, unsigned int height) 
: World()
, width_(width)
, height_(height) {
}

void ServerWorld::update(float elapsed) {
    World::update(elapsed);

    std::set<uint32_t> objectIds;
    forEachGameObject([&objectIds, this] (uint32_t objectId, GameObject* gameObject) {
        const auto& pos = gameObject->getPosition();
        if ((pos.x() < 40 || pos.y() < 40 || pos.x() > (width_-40) || pos.y() > (height_-40)) && gameObject->getClassId() != SpaceShip::ClassId) {
            objectIds.insert(objectId);
        }
    });
    for (const auto objectId : objectIds) {
        remove(objectId);
    }
}
