#include "RemoteSpaceShip.h"
#include "LatencyEstimator.h"

RemoteSpaceShip::RemoteSpaceShip(const Renderer& renderer, const LatencyEstimator& latencyEstimator, float frameDuration)
: SpaceShip(renderer)
, latencyEstimator_(latencyEstimator)
, frameDuration_(frameDuration) {
}

void RemoteSpaceShip::update(float elapsed) {
    SpaceShip::update(elapsed);
}

void RemoteSpaceShip::read(Packet* packet) {
    SpaceShip::read(packet);

    auto latency = latencyEstimator_.getMeanRTT();
    while (true) {
        if (latency < frameDuration_) {
            update(latency);
            break;
        } else {
            update(frameDuration_);
            latency -= frameDuration_;
        }
    }
}
