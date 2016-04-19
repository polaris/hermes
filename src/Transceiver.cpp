#include "Transceiver.h"

#include <iostream>

Transceiver::Transceiver(Queue<Buffer>& bufferPool, Queue<Buffer>& incomingPackets)
: bufferPool_(bufferPool)
, incomingPackets_(incomingPackets)
, io_service_()
, work_(io_service_)
, socket_(io_service_, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0))
, thread_([this] () { io_service_.run(); }) {
}

void Transceiver::sendTo(Buffer* buffer) {
    socket_.async_send_to(boost::asio::buffer(buffer->data(), buffer->size()), buffer->endpoint(),
        [this, buffer] (const boost::system::error_code &ec, std::size_t bytesTransferred) {
            if (ec) {
                std::cerr << ec.message() << std::endl;
            } else {
                std::cout << bytesTransferred << " bytes transferred" << std::endl;
                assert(bytesTransferred == buffer->size());
            }
            bufferPool_.push(buffer);
        });
}

void Transceiver::receiveFrom(Buffer* buffer) {
    socket_.async_receive_from(boost::asio::buffer(buffer->data(), buffer->capacity()), buffer->endpoint(),
        [this, buffer] (const boost::system::error_code &ec, std::size_t bytesReceived) {
            if (ec) {
                std::cerr << ec.message() << std::endl;
                bufferPool_.push(buffer);
            } else {
                buffer->size(bytesReceived);
                std::cout << buffer->size() << " bytes received from " << buffer->endpoint().address() << ":" << buffer->endpoint().port() << std::endl;
                incomingPackets_.push(buffer);

                auto newBuffer = bufferPool_.pop();
                if (newBuffer) {
                    receiveFrom(newBuffer);
                } else {
                    // TODO: Handle out-of-buffers
                }
            }
        });
}

Transceiver::~Transceiver() {
    io_service_.stop();
    thread_.join();
}
