#ifndef _LocalSpaceShip_H
#define _LocalSpaceShip_H

#include "SpaceShip.h"

#include <functional>

class InputHandler;

using ShootFunc = std::function<float (SpaceShip*, float)>;

class LocalSpaceShip : public SpaceShip {
public:
    LocalSpaceShip(const Renderer& renderer, InputHandler& inputHandler, ShootFunc shootFunc);

    LocalSpaceShip(const Renderer& renderer, InputHandler& inputHandler);

    void update(float elapsed) override;

    void draw(Renderer& renderer) override;

    void read(Packet* packet) override;

private:
    InputHandler& inputHandler_;

    ShootFunc shootFunc_;

    float lastShot_;

    const unsigned int length;
};

#endif  // _LocalSpaceShip_H
