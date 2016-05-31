#include "RemoteLaserBolt.h"
#include "LatencyEstimator.h"
#include "Utilities.h"

RemoteLaserBolt::RemoteLaserBolt(const Renderer& renderer, const LatencyEstimator& latencyEstimator, float frameDuration)
: LaserBolt(renderer, Vector2d(0, 0), Vector2d(0, 0))
, latencyEstimator_(latencyEstimator)
, frameDuration_(frameDuration)
, created_(true) {
}

void RemoteLaserBolt::read(Packet* packet) {
    const auto oldPosition = position_;

    LaserBolt::read(packet);

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
    }
}
