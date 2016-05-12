#include "RemoteSpaceShip.h"
#include "RollingMean.h"

RemoteSpaceShip::RemoteSpaceShip(const Renderer& renderer, const RollingMean& rollingMean, float frameDuration)
: SpaceShip(renderer)
, rollingMeanRrt_(rollingMean)
, frameDuration_(frameDuration) {
}

void RemoteSpaceShip::update(float elapsed) {
    SpaceShip::update(elapsed);
}

void RemoteSpaceShip::read(Packet* packet) {
    SpaceShip::read(packet);

    auto latency = rollingMeanRrt_.getMean();
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
