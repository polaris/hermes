#include "LatencyEmulator.h"

#include <memory>

LatencyEmulator::LatencyEmulator(PacketSink& packetSink, unsigned int latencyMs)
: packetSink_(packetSink)
, latencyMs_(latencyMs)
, io_service_()
, work_(io_service_)
, thread_([this] () { io_service_.run(); }) {
}

LatencyEmulator::~LatencyEmulator() {
    io_service_.stop();
    thread_.join();
}

void LatencyEmulator::push(Packet* packet) {
    packetSink_.push(packet);
}

Packet* LatencyEmulator::pop() {
    return packetSink_.pop();
}

void LatencyEmulator::enqueue(Packet* packet) {
    auto timer = std::make_shared<boost::asio::deadline_timer>(io_service_);
    timer->expires_from_now(boost::posix_time::milliseconds(latencyMs_));
    timer->async_wait([this, timer, packet] (const boost::system::error_code&) {
        // TODO: Handle error.
        packetSink_.enqueue(packet);                    
    });
}

Packet* LatencyEmulator::dequeue() {
    return packetSink_.dequeue();
}
