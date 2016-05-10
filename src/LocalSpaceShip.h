#ifndef _LocalSpaceShip_H
#define _LocalSpaceShip_H

#include "SpaceShip.h"

class InputHandler;

class LocalSpaceShip : public SpaceShip {
public:
    LocalSpaceShip(const Renderer& renderer, InputHandler& inputHandler);

    void update(float elapsed) override;

    void read(Packet* packet) override;

private:
    InputHandler& inputHandler_;
};

#endif  // _LocalSpaceShip_H
