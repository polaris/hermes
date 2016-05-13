#ifndef _LatencyEstimator_H
#define _LatencyEstimator_H

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>

class LatencyEstimator {
public:
    explicit LatencyEstimator(unsigned int windowSize);

    void addRTT(float value);

    float getMeanRTT() const;

private:
    boost::accumulators::accumulator_set<float, boost::accumulators::stats<boost::accumulators::tag::rolling_mean>> accu_;
};

#endif  // _LatencyEstimator_H
