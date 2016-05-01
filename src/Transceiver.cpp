#include "Transceiver.h"

#include <spdlog/spdlog.h>
#include <boost/format.hpp>

Transceiver::Transceiver(uint16_t port, Queue<Packet>& packetPool, Queue<Packet>& incomingPackets)
: packetPool_(packetPool)
, incomingPackets_(incomingPackets)
, io_service_()
, work_(io_service_)
, socket_(io_service_, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port))
, thread_([this] () { io_service_.run(); }) {
    auto packet = packetPool_.pop();
    assert(packet != nullptr);
    if (packet) {
        packet->clear();
        receiveFrom(packet);
    }
}

Transceiver::Transceiver(Queue<Packet>& packetPool, Queue<Packet>& incomingPackets)
: Transceiver(0, packetPool, incomingPackets) {
}

void Transceiver::sendTo(Packet* packet) {
    socket_.async_send_to(boost::asio::buffer(packet->getData(), packet->getSize()), packet->getEndpoint(),
        [this, packet] (const boost::system::error_code &ec, std::size_t bytesTransferred) {
            if (ec) {
                const auto logMessage = boost::str(boost::format("Failed to send a packet: %1%") % ec.message());
                spdlog::get("console")->error(logMessage);
            } else {
                assert(bytesTransferred == packet->getSize());
            }
            packetPool_.push(packet);
        });
}

void Transceiver::receiveFrom(Packet* packet) {
    socket_.async_receive_from(boost::asio::buffer(packet->getData(), packet->getCapacity()), packet->getEndpoint(),
        [this, packet] (const boost::system::error_code &ec, std::size_t bytesReceived) {
            if (ec) {
                const auto logMessage = boost::str(boost::format("Failed to receive a packet: %1%") % ec.message());
                spdlog::get("console")->error(logMessage);
                packetPool_.push(packet);
            } else {
                auto newBuffer = packetPool_.pop();
                if (newBuffer) {
                    packet->setSize(static_cast<uint32_t>(bytesReceived));
                    incomingPackets_.push(packet);
                    newBuffer->clear();
                    receiveFrom(newBuffer);
                } else {
                    spdlog::get("console")->error("Packet pool is empty. Discard the received packet.");
                    packet->clear();
                    receiveFrom(packet);
                }
            }
        });
}

Transceiver::~Transceiver() {
    io_service_.stop();
    thread_.join();
}
