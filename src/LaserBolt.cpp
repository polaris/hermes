#include "LaserBolt.h"

LaserBolt::LaserBolt(const Renderer& renderer, const Vector2d& position, const Vector2d& velocity)
: sprite_("data/shoot1.png", renderer)
, position_(position)
, velocity_(velocity)
, lookat_(velocity_.normal()) {
}

void LaserBolt::update(float elapsed) {
    position_ += (elapsed * velocity_);
}

void LaserBolt::draw(Renderer &renderer) {
    sprite_.draw(
        static_cast<int>(position_.getX() - static_cast<float>(sprite_.getWidth()) / 2.0f),
        static_cast<int>(position_.getY() - static_cast<float>(sprite_.getHeight()) / 2.0f),
        -angle(Vector2d{0, -1}, lookat_),
        renderer);
}

void LaserBolt::write(Packet* packet) {
    position_.write(packet);
    velocity_.write(packet);
    lookat_.write(packet);
}

void LaserBolt::read(Packet* packet) {
    position_.read(packet);
    velocity_.read(packet);
    lookat_.read(packet);
}

const Vector2d& LaserBolt::getPosition() const {
    return position_;
}

unsigned int LaserBolt::getWidth() const {
    return sprite_.getWidth();
}

unsigned int LaserBolt::getHeight() const {
    return sprite_.getHeight();
}

uint32_t LaserBolt::getClassId() const {
    return ClassId;
}
