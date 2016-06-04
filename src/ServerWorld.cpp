#include "ServerWorld.h"
#include "Vector2d.h"
#include "SpaceShip.h"
#include "Logging.h"

#include <boost/format.hpp>

#include <unordered_set>
#include <string>

ServerWorld::ServerWorld(unsigned int width, unsigned int height, ConfirmCollisionFunc confirmCollisionFunc, RemovedObjectFunc removedObjectFunc)
: World()
, width_(width)
, height_(height)
, confirmCollisionFunc_(confirmCollisionFunc)
, removedObjectFunc_(removedObjectFunc) {
}

void ServerWorld::update(float elapsed) {
    World::update(elapsed);

    std::unordered_set<std::string> checked;

    forEachGameObject([this, &checked] (uint32_t objectId1, GameObject* gameObject1) {
        const auto& pos = gameObject1->getPosition();
        if ((pos.x() < 0 || pos.y() < 0 || pos.x() > width_ || pos.y() > height_) && gameObject1->getClassId() != SpaceShip::ClassId) {
            gameObject1->kill();
        }
        if (!gameObject1->doesCollide()) {
            return;
        }
        forEachGameObject([this, objectId1, gameObject1, &checked] (uint32_t objectId2, GameObject* gameObject2) {
            if (!gameObject2->doesCollide()) {
                return;
            }
            auto oid1 = objectId1;
            auto oid2 = objectId2;
            if (oid1 != oid2) {
                if (oid1 > oid2) {
                    std::swap(oid1, oid2);
                }
                const auto id = boost::str(boost::format("%1%,%2%") % oid1 % oid2);
                if (checked.find(id) == checked.end()) {
                    if (gameObject1->checkCollision(gameObject2) && confirmCollisionFunc_(oid1, gameObject1, oid2, gameObject2)) {
                        gameObject1->kill();
                        gameObject2->kill();
                    }
                    checked.insert(id);
                }
            }
        });
    });

    removeIf([this] (uint32_t objectId, GameObject* gameObject) {
        if (gameObject->dead()) {
            removedObjectFunc_(objectId);
            return true;
        }
        return false;
    });
}
