#ifndef _ServerSpaceShip_H
#define _ServerSpaceShip_H

#include "SpaceShip.h"

class Renderer;
class ClientSession;

class ServerSpaceShip : public SpaceShip {
public:
    ServerSpaceShip(const Renderer& renderer, ClientSession* clientSession);

    ServerSpaceShip(const ServerSpaceShip&) = delete;

    ServerSpaceShip& operator =(const ServerSpaceShip&) = delete;

    void update(float elapsed) override;

private:
    ClientSession* clientSession_;
};

#endif  // _ServerSpaceShip_H
