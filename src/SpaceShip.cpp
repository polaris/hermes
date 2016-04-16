#include "SpaceShip.h"

SpaceShip::SpaceShip(const Renderer &renderer)
: sprite_("data/ninja.png", renderer)
, position_(320, 240)
, velocity_(0, 0)
, lookat_(0, 1)
, acceleration_(0) {
}

void SpaceShip::rotate(float angle) {
    lookat_.rotate(angle);
}

void SpaceShip::update(float elapsed) {
    velocity_ += (elapsed * (acceleration_ * lookat_));
    position_ += (elapsed * velocity_);
}

void SpaceShip::draw(const Renderer &renderer) {
    sprite_.draw(
        static_cast<int>(position_.x() - sprite_.getWidth() / 2),
        static_cast<int>(position_.y() - sprite_.getHeight() / 2),
        angle(Vector2d{0, 1}, lookat_),
        renderer);
}
