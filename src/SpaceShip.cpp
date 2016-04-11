#include "SpaceShip.h"

SpaceShip::SpaceShip(const Renderer &renderer)
: sprite_("data/ninja.png", renderer) {
}

void SpaceShip::update(float /*elapsed*/) {

}

void SpaceShip::draw(const Renderer &renderer) {
    sprite_.draw(0, 0, 56, 80, renderer);
}
