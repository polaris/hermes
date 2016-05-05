#ifndef _LatencyEmulator_H
#define _LatencyEmulator_H

#include "PacketSink.h"

#include <boost/asio.hpp>

#include <thread>

class LatencyEmulator : public PacketSink {
public:
    LatencyEmulator(PacketSink& packetSink, unsigned int latencyMs);

    ~LatencyEmulator();

    void push(Packet* packet) override;

    Packet* pop() override;

    void enqueue(Packet* packet) override;

    Packet* dequeue() override;

private:
    PacketSink& packetSink_;
    unsigned int latencyMs_;
    boost::asio::io_service io_service_;
    boost::asio::io_service::work work_;
    std::thread thread_;
};

#endif  // _LatencyEmulator_H
