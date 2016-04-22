#include "Transceiver.h"

#include <iostream>

Transceiver::Transceiver(unsigned short port, Queue<Buffer>& bufferPool, Queue<Buffer>& incomingPackets)
: bufferPool_(bufferPool)
, incomingPackets_(incomingPackets)
, io_service_()
, work_(io_service_)
, socket_(io_service_, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port))
, thread_([this] () { io_service_.run(); }) {
    auto buffer = bufferPool_.pop();
    assert(buffer != nullptr);
    if (buffer) {
        receiveFrom(buffer);
    }
}

Transceiver::Transceiver(Queue<Buffer>& bufferPool, Queue<Buffer>& incomingPackets)
: Transceiver(0, bufferPool, incomingPackets) {
}

void Transceiver::sendTo(Buffer* buffer) {
    socket_.async_send_to(boost::asio::buffer(buffer->data(), buffer->size()), buffer->getEndpoint(),
        [this, buffer] (const boost::system::error_code &ec, std::size_t bytesTransferred) {
            if (ec) {
                std::cerr << ec.message() << "\n";
            } else {
                assert(bytesTransferred == buffer->size());
            }
            buffer->reset();
            bufferPool_.push(buffer);
        });
}

void Transceiver::receiveFrom(Buffer* buffer) {
    socket_.async_receive_from(boost::asio::buffer(buffer->data(), buffer->capacity()), buffer->getEndpoint(),
        [this, buffer] (const boost::system::error_code &ec, std::size_t bytesReceived) {
            if (ec) {
                std::cerr << ec.message() << "\n";
                buffer->reset();
                bufferPool_.push(buffer);
            } else {
                auto newBuffer = bufferPool_.pop();
                if (newBuffer) {
                    buffer->size(bytesReceived);
                    incomingPackets_.push(buffer);
                    receiveFrom(newBuffer);
                } else {
                    std::cerr << "Buffer pool is empty. Discard the received packet.\n";
                    buffer->reset();
                    receiveFrom(buffer);
                }
            }
        });
}

Transceiver::~Transceiver() {
    io_service_.stop();
    thread_.join();
}
