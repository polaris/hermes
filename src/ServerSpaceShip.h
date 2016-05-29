#ifndef _ServerSpaceShip_H
#define _ServerSpaceShip_H

#include "SpaceShip.h"

#include <functional>

class Renderer;
class ClientSession;
class ServerSpaceShip;

using ShootFunc = std::function<float (SpaceShip*, float)>;

class ServerSpaceShip : public SpaceShip {
public:
    ServerSpaceShip(const Renderer& renderer, const Vector2d& position, ClientSession* clientSession, ShootFunc shootFunc);

    ServerSpaceShip(const ServerSpaceShip&) = delete;

    ServerSpaceShip& operator =(const ServerSpaceShip&) = delete;

    void update(float elapsed) override;

private:
    ClientSession* clientSession_;

    ShootFunc shootFunc_;

    float lastShot_;
};

#endif  // _ServerSpaceShip_H
