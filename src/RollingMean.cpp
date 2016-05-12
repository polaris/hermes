#include "RollingMean.h"

RollingMean::RollingMean(unsigned int windowSize)
: accu_(boost::accumulators::tag::rolling_window::window_size = windowSize) {
}

void RollingMean::addValue(float value) {
    accu_(value);
}

float RollingMean::getMean() const {
    return boost::accumulators::rolling_mean(accu_);
}
