#ifndef _LocalSpaceShip_H
#define _LocalSpaceShip_H

#include "SpaceShip.h"

class Renderer;
class InputHandler;

class LocalSpaceShip : public SpaceShip {
public:
    LocalSpaceShip(const Renderer &renderer, InputHandler& inputHandler);

private:
    InputHandler& inputHandler_;

    void beforeUpdate(float elapsed) override;
};

#endif  // _LocalSpaceShip_H
