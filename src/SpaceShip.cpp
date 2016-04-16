#include "SpaceShip.h"
#include "Renderer.h"

SpaceShip::SpaceShip(const Renderer &renderer)
: sprite_("data/ship.png", renderer)
, position_(320, 240)
, velocity_(0, 0)
, lookat_(0, -1)
, acceleration_(0) {
}

void SpaceShip::rotate(float angle) {
    lookat_.rotate(angle);
}

void SpaceShip::thrust(bool onOff) {
    acceleration_ = onOff ? 50.0f : 0.0f;
}

void SpaceShip::update(float elapsed) {
    velocity_ += (elapsed * (acceleration_ * lookat_));
    position_ += (elapsed * velocity_);
}

void SpaceShip::draw(const Renderer &renderer) {
    const auto v = 30 * lookat_;
    SDL_RenderDrawLine(renderer.getSDLRenderer(),
                       static_cast<int>(position_.x()),
                       static_cast<int>(position_.y()),
                       static_cast<int>(position_.x() + v.x()),
                       static_cast<int>(position_.y() + v.y()));
    SDL_RenderDrawLine(renderer.getSDLRenderer(),
                       static_cast<int>(position_.x()),
                       static_cast<int>(position_.y()),
                       static_cast<int>(position_.x() + velocity_.x()),
                       static_cast<int>(position_.y() + velocity_.y()));
    sprite_.draw(
        static_cast<int>(position_.x() - sprite_.getWidth() / 2),
        static_cast<int>(position_.y() - sprite_.getHeight() / 2),
        -angle(Vector2d{0, -1}, lookat_),
        renderer);
}
