#include "ServerWorld.h"
#include "Vector2d.h"
#include "SpaceShip.h"
#include "Logging.h"

#include <unordered_set>
#include <tuple>

ServerWorld::ServerWorld(unsigned int width, unsigned int height) 
: World()
, width_(width)
, height_(height) {
}

void ServerWorld::update(float elapsed) {
    World::update(elapsed);

    std::unordered_set<std::tuple<uint32_t, uint32_t>> checked;

    forEachGameObject([this, &checked] (uint32_t objectId1, GameObject* gameObject1) {
        forEachGameObject([this, objectId1, gameObject1, &checked] (uint32_t objectId2, GameObject* gameObject2) {
            auto oid1 = objectId1;
            auto oid2 = objectId2;
            if (oid1 != oid2) {
                if (oid1 > oid2) {
                    std::swap(oid1, oid2);
                }
                const auto tuple = std::make_tuple(oid1, oid2);
                if (checked.find(tuple) != checked.end()) {
                    if (gameObject1->checkCollision(gameObject2)) {
                        ERROR("{0} collides with {1}", oid1, oid2);
                    }
                    checked.insert(tuple);
                }
            }
        });
    });

    std::unordered_set<uint32_t> objectIds;
    forEachGameObject([&objectIds, this] (uint32_t objectId, GameObject* gameObject) {
        const auto& pos = gameObject->getPosition();
        if ((pos.x() < 0 || pos.y() < 0 || pos.x() > width_ || pos.y() > height_) && gameObject->getClassId() != SpaceShip::ClassId) {
            objectIds.insert(objectId);
        }
    });
    for (const auto objectId : objectIds) {
        remove(objectId);
    }
}
