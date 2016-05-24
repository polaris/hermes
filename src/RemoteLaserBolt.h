#ifndef _RemoteLaserBolt_H
#define _RemoteLaserBolt_H

#include "LaserBolt.h"

class LatencyEstimator;

class RemoteLaserBolt : public LaserBolt {
public:
    RemoteLaserBolt(const Renderer& renderer, const LatencyEstimator& latencyEstimator, float frameDuration);

    void read(Packet* packet) override;

private:
    const LatencyEstimator& latencyEstimator_;

    const float frameDuration_;
};

#endif  // _RemoteLaserBolt_H
