#include "LatencyEstimator.h"

LatencyEstimator::LatencyEstimator(unsigned int windowSize)
: accu_(boost::accumulators::tag::rolling_window::window_size = windowSize) {
}

void LatencyEstimator::addRTT(float value) {
    accu_(value);
}

float LatencyEstimator::getMeanRTT() const {
    return boost::accumulators::rolling_mean(accu_);
}
