#ifndef _RollingMean_H
#define _RollingMean_H

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>

class RollingMean {
public:
    explicit RollingMean(unsigned int windowSize);

    void addValue(float value);

    float getMean();

private:
    boost::accumulators::accumulator_set<float, boost::accumulators::stats<boost::accumulators::tag::rolling_mean>> accu_;
};

#endif  // _RollingMean_H
