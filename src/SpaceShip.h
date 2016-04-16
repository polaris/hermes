#ifndef _SpaceShip_H
#define _SpaceShip_H

#include "GameObject.h"
#include "Sprite.h"
#include "Vector2d.h"

class SpaceShip : public GameObject {
public:
    SpaceShip(const Renderer& renderer);

    void rotate(float angle);

    void update(float elapsed);

    void draw(const Renderer& renderer);

private:
    Sprite sprite_;
    Vector2d position_;
    Vector2d velocity_;
    Vector2d lookat_;
    float acceleration_;
};

#endif  // _SpaceShip_H
