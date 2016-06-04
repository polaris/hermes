#include "GameObject.h"
#include "Vector2d.h"

#include <SDL2/SDL.h>

GameObject::GameObject()
: dead_(false) {
}

bool GameObject::checkCollision(GameObject* gameObject) const {
    const auto& pos1 = getPosition();
    const SDL_Rect rect1 = {
        static_cast<int>(pos1.x()),
        static_cast<int>(pos1.y()),
        static_cast<int>(getWidth()),
        static_cast<int>(getHeight())
    };

    const auto& pos2 = gameObject->getPosition();
    const SDL_Rect rect2 = {
        static_cast<int>(pos2.x()),
        static_cast<int>(pos2.y()),
        static_cast<int>(gameObject->getWidth()),
        static_cast<int>(gameObject->getHeight())
    };

    return SDL_HasIntersection(&rect1, &rect2) == SDL_TRUE;
}

void GameObject::kill() {
    dead_ = true;
}

bool GameObject::dead() const {
    return dead_;
}

bool GameObject::doesCollide() const {
    return true;
}
