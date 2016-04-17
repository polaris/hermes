#ifndef _SpaceShip_H
#define _SpaceShip_H

#include "GameObject.h"
#include "Sprite.h"
#include "Vector2d.h"

class Renderer;
class InputHandler;

class SpaceShip : public GameObject {
public:
    explicit SpaceShip(const Renderer& renderer, InputHandler& inputHandler);

    void update(float elapsed);

    void draw(Renderer& renderer);

protected:
    void rotate(float angle);

    void thrust(bool onOff);

private:
    Sprite sprite_;
    InputHandler& inputHandler_;

    Vector2d position_;
    Vector2d velocity_;
    Vector2d lookat_;
    float acceleration_;
};

#endif  // _SpaceShip_H
