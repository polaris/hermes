#include "RemoteSpaceShip.h"
#include "LatencyEstimator.h"
#include "Utilities.h"

RemoteSpaceShip::RemoteSpaceShip(const Renderer& renderer, const LatencyEstimator& latencyEstimator, float frameDuration)
: SpaceShip(renderer)
, latencyEstimator_(latencyEstimator)
, frameDuration_(frameDuration)
, created_(true) {
}

void RemoteSpaceShip::update(float elapsed) {
    SpaceShip::update(elapsed);
}

void RemoteSpaceShip::read(Packet* packet) {
    const auto oldPosition = position_;
    const auto oldVelocity = velocity_;
    const auto oldLookat = lookat_;
    const auto oldAcceleration = acceleration_;

    SpaceShip::read(packet);

    auto latency = latencyEstimator_.getMeanRTT() / 2.0f;
    while (true) {
        if (latency < frameDuration_) {
            update(latency);
            break;
        } else {
            update(frameDuration_);
            latency -= frameDuration_;
        }
    }

    if (created_) {
        created_ = false;
    } else {
        position_ = lerp(position_, oldPosition, 0.5f);
        velocity_ = lerp(velocity_, oldVelocity, 0.5f);
        lookat_ = lerp(lookat_, oldLookat, 0.5f);
        acceleration_ = lerp(acceleration_, oldAcceleration, 0.5f);
    }
}
