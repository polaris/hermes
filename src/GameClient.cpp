#include "GameClient.h"

#include <iostream>

GameClient::GameClient(unsigned int poolSize)
: bufferPool(poolSize)
, bufferQueue(poolSize)
, io_service()
, work(io_service)
, socket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0))
, thread([this] () { io_service.run(); }) {

    initBufferPool();

    auto buffer = bufferPool.pop();
    buffer->clear();
    buffer->size(32);
    send("127.0.0.1", 12345, buffer);
}

void GameClient::initBufferPool() {
    for (unsigned int i = 0; i < bufferPool.getSize(); ++i) {
        bufferPool.push(new Buffer(1024));
    }
}

void GameClient::send(const char* address, unsigned short port, Buffer* sendBuffer) {
    boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::address::from_string(address), port);
    socket.async_send_to(boost::asio::buffer(sendBuffer->data(), sendBuffer->size()), endpoint,
        [this, sendBuffer] (const boost::system::error_code &ec, std::size_t bytesTransferred) {
            if (ec) {
                std::cerr << ec.message() << std::endl;
            } else {
                std::cout << bytesTransferred << " bytes transferred" << std::endl;
                assert(bytesTransferred == sendBuffer->size());

                auto receiveBuffer = bufferPool.pop();
                receiveBuffer->reset();
                receive(receiveBuffer);
            }

            bufferPool.push(sendBuffer);
        });
}

void GameClient::receive(Buffer *receiveBuffer) {
    socket.async_receive_from(boost::asio::buffer(receiveBuffer->data(), receiveBuffer->capacity()), senderEndpoint,
        [this, receiveBuffer] (const boost::system::error_code &ec, std::size_t bytesReceived) {
            if (ec) {
                std::cerr << ec.message() << std::endl;
            } else {
                receiveBuffer->size(bytesReceived);
                std::cout << receiveBuffer->size() << " bytes received from " << senderEndpoint.address() << ":" << senderEndpoint.port() << std::endl;

                auto sendBuffer = bufferPool.pop();
                sendBuffer->clear();
                sendBuffer->size(512);
                send("127.0.0.1", 12345, sendBuffer);
            }

            bufferPool.push(receiveBuffer);
        });
}

GameClient::~GameClient() {
    io_service.stop();
    thread.join();
}
