#include "Transceiver.h"
#include "Packet.h"
#include "PacketSink.h"
#include "Logging.h"

Transceiver::Transceiver(uint16_t port, PacketSink& packetSink)
: packetSink_(packetSink)
, io_service_()
, work_(io_service_)
, socket_(io_service_, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port))
, thread_([this] () { io_service_.run(); }) {
    auto packet = packetSink_.pop();
    assert(packet != nullptr);
    if (packet) {
        packet->clear();
        receiveFrom(packet);
    }
}

Transceiver::Transceiver(PacketSink& packetSink)
: Transceiver(0, packetSink) {
}

void Transceiver::sendTo(Packet* packet) {
    socket_.async_send_to(boost::asio::buffer(packet->getData(), packet->getSize()), packet->getEndpoint(),
        [this, packet] (const boost::system::error_code &ec, std::size_t bytesTransferred) {
            if (ec) {
                ERROR("Failed to send a packet: {0}", ec.message());
            } else {
                assert(bytesTransferred == packet->getSize());
            }
            packetSink_.push(packet);
        });
}

void Transceiver::receiveFrom(Packet* packet) {
    socket_.async_receive_from(boost::asio::buffer(packet->getData(), packet->getCapacity()), packet->getEndpoint(),
        [this, packet] (const boost::system::error_code &ec, std::size_t bytesReceived) {
            if (ec) {
                ERROR("Failed to receive a packet: {0}", ec.message());
                packetSink_.push(packet);
            } else {
                auto newBuffer = packetSink_.pop();
                if (newBuffer) {
                    packet->setSize(static_cast<uint32_t>(bytesReceived));
                    packetSink_.enqueue(packet);
                    newBuffer->clear();
                    receiveFrom(newBuffer);
                } else {
                    WARN("Packet pool is empty. Discard the received packet.");
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
