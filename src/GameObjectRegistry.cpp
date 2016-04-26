#include "GameObjectRegistry.h"
#include "SpaceShip.h"

void registerGameObjects() {
    GameObjectRegistry::get().registerFunction<SpaceShip>();
}