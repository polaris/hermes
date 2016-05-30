#ifndef _LatencyEmulator_H
#define _LatencyEmulator_H

#include "PacketSink.h"

#include <boost/asio.hpp>

#include <thread>
#include <random>

class LatencyEmulator : public PacketSink {
public:
    LatencyEmulator(PacketSink& packetSink, unsigned int latencyMs, unsigned int stdDevMean);

    ~LatencyEmulator();

    void push(Packet* packet) override;

    Packet* pop() override;

    void enqueue(Packet* packet) override;

    Packet* dequeue() override;

private:
    PacketSink& packetSink_;

    const unsigned int latencyMs_;
    std::random_device randomDevice_;
    std::mt19937 randomGenerator_;
    std::normal_distribution<> normalDistribution_;

    boost::asio::io_service io_service_;
    boost::asio::io_service::work work_;
    std::thread thread_;
};

#endif  // _LatencyEmulator_H
