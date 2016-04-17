#include "World.h"

World::World() {
}

void World::add(GameObjectPtr&& gameObject) {
    gameObjects.push_back(gameObject);
}

void World::add(GameObjectPtr& gameObject) {
    gameObjects.push_back(gameObject);
}

void World::remove(GameObjectPtr& gameObject) {
    auto itr = std::find(gameObjects.begin(), gameObjects.end(), gameObject);
    if (itr != gameObjects.end()) {
        gameObjects.erase(itr);
    }
}

void World::update(float elapsed) {
    for (auto& gameObject : gameObjects) {
        gameObject->update(elapsed);
    }
}

void World::draw(Renderer& renderer) {
    for (auto& gameObject : gameObjects) {
        gameObject->draw(renderer);
    }
}
