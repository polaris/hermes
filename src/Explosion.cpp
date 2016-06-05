#include "Explosion.h"
#include "Logging.h"
#include "Sound.h"

Explosion::Explosion(const Renderer& renderer, const Vector2d& position)
: animation_("data/explosion1.png", 12, 12, renderer)
, position_(position) {
    Sound::getInstance()->playSound(1);
}

void Explosion::update(float elapsed) {
    if (animation_.update(elapsed)) {
        kill();
    }
}

void Explosion::draw(Renderer& renderer) {
    animation_.draw(static_cast<int>(position_.x()), static_cast<int>(position_.y()), renderer);
}

void Explosion::write(Packet* packet) {
    position_.write(packet);
}

void Explosion::read(Packet* packet) {
    position_.read(packet);
}

const Vector2d& Explosion::getPosition() const {
    return position_;
}

unsigned int Explosion::getWidth() const {
    return animation_.getWidth();
}

unsigned int Explosion::getHeight() const {
    return animation_.getHeight();
}

bool Explosion::doesCollide() const {
    return false;
}

uint32_t Explosion::getClassId() const {
    return ClassId;
}
