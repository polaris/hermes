#include "SpaceShip.h"
#include "Renderer.h"
#include "InputHandler.h"
#include "Packet.h"

SpaceShip::SpaceShip(const Renderer &renderer)
: sprite_("data/ship.png", renderer)
, position_(320, 240)
, velocity_(0, 0)
, lookat_(0, -1)
, acceleration_(0) {
}

void SpaceShip::update(float elapsed) {
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

void SpaceShip::write(Packet* packet) {
    position_.write(packet);
    velocity_.write(packet);
    lookat_.write(packet);
    packet->write(acceleration_);
}

void SpaceShip::read(Packet* packet) {
    position_.read(packet);
    velocity_.read(packet);
    lookat_.read(packet);
    packet->read(acceleration_);
}

unsigned int SpaceShip::getClassId() const {
    return ClassId;
}

void SpaceShip::rotate(float angle) {
    lookat_.rotate(angle);
}

void SpaceShip::thrust(bool onOff) {
    acceleration_ = onOff ? 50.0f : 0.0f;
}

GameObjectPtr SpaceShip::createInstance(const Renderer& renderer) {
    return GameObjectPtr(new SpaceShip(renderer));
}
