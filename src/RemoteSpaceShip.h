#ifndef _RemoteSpaceShip_H
#define _RemoteSpaceShip_H

#include "SpaceShip.h"

class LatencyEstimator;

class RemoteSpaceShip : public SpaceShip {
public:
    RemoteSpaceShip(const Renderer& renderer, const LatencyEstimator& latencyEstimator, float frameDuration);

    void update(float elapsed) override;

    void read(Packet* packet) override;

private:
    const LatencyEstimator& latencyEstimator_;

    const float frameDuration_;
};

#endif  // _RemoteSpaceShip_H
