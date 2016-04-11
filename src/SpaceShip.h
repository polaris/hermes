#ifndef _SpaceShip_H
#define _SpaceShip_H

#include "GameObject.h"
#include "Sprite.h"

class SpaceShip : public GameObject {
public:
    SpaceShip(const Renderer &renderer);

    void update(float elapsed);

    void draw(const Renderer &renderer);

private:
    Sprite sprite_;
};

#endif  // _SpaceShip_H
