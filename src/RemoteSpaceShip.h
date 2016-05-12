#ifndef __RemoteSpaceShip_H
#define __RemoteSpaceShip_H

#include "SpaceShip.h"

class RollingMean;

class RemoteSpaceShip : public SpaceShip {
public:
    RemoteSpaceShip(const Renderer& renderer, const RollingMean& rollingMean, float frameDuration);

    void update(float elapsed) override;

    void read(Packet* packet) override;

private:
    const RollingMean& rollingMeanRrt_;
    float frameDuration_;
};

#endif  // __RemoteSpaceShip_H
