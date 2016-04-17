#include "SpaceShip.h"
#include "Renderer.h"
#include "InputHandler.h"

SpaceShip::SpaceShip(const Renderer &renderer, InputHandler& inputHandler)
: sprite_("data/ship.png", renderer)
, inputHandler_(inputHandler)
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
    auto move = inputHandler_.getAndClearPendingMove();
    if (move) {
        const auto& inputState = move->getInputState();
        rotate(inputState.desiredRightAmount * 5 * elapsed);
        rotate(-inputState.desiredLeftAmount * 5 * elapsed);
        thrust(inputState.desiredForwardAmount > 0);
    }

    velocity_ *= 0.995f;
    velocity_ += (elapsed * (acceleration_ * lookat_));
    position_ += (elapsed * velocity_);
}

void SpaceShip::draw(Renderer &renderer) {
    renderer.setDrawColor(1, 1, 1, 1);
    renderer.drawLine(position_, position_ + (30 * lookat_));

    renderer.setDrawColor(1, 0, 0, 1);
    renderer.drawLine(position_, position_ + velocity_);

    sprite_.draw(
        static_cast<int>(position_.x() - sprite_.getWidth() / 2),
        static_cast<int>(position_.y() - sprite_.getHeight() / 2),
        -angle(Vector2d{0, -1}, lookat_),
        renderer);
}
