#include "SpaceShip.h"

#include <iostream>

SpaceShip::SpaceShip(const Renderer &renderer)
: sprite_("data/ninja.png", renderer)
, velocity_(25.0f, 25.0f) {
}

void SpaceShip::update(float elapsed) {
    position_ += (elapsed * velocity_);
}

void SpaceShip::draw(const Renderer &renderer) {
    sprite_.draw(static_cast<int>(position_.x()), static_cast<int>(position_.y()), renderer);
}
