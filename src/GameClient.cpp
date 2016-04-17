#include "GameClient.h"

#include <iostream>

GameClient::GameClient(const char *address, unsigned short port, unsigned int poolSize)
: bufferPool(poolSize)
, bufferQueue(poolSize)
, io_service()
, work(io_service)
, socket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0))
, endpoint(boost::asio::ip::address::from_string(address), port)
, thread([this] () { io_service.run(); }) {

    initBufferPool();

    auto buffer = bufferPool.pop();
    buffer->clear();
    buffer->size(32);
    send(buffer);
}

void GameClient::initBufferPool() {
    for (unsigned int i = 0; i < bufferPool.getSize(); ++i) {
        bufferPool.push(new Buffer(1024));
    }
}

void GameClient::send(Buffer *sendBuffer) {
    socket.async_send_to(boost::asio::buffer(sendBuffer->data(), sendBuffer->size()), endpoint,
        [this, sendBuffer] (const boost::system::error_code &ec, std::size_t bytesTransferred) {
            if (ec) {
                std::cerr << ec.message() << std::endl;
            } else {
                std::cout << bytesTransferred << " bytes transferred" << std::endl;
                assert(bytesTransferred == sendBuffer->size());
                bufferPool.push(sendBuffer);

                auto receiveBuffer = bufferPool.pop();
                receiveBuffer->reset();
                receive(receiveBuffer);
            }
        });
}

void GameClient::receive(Buffer *receiveBuffer) {
    socket.async_receive_from(boost::asio::buffer(receiveBuffer->data(), receiveBuffer->capacity()), endpoint,
        [this, receiveBuffer] (const boost::system::error_code &ec, std::size_t bytesReceived) {
            if (ec) {
                std::cerr << ec.message() << std::endl;
            } else {
                receiveBuffer->size(bytesReceived);
                std::cout << receiveBuffer->size() << " bytes received" << std::endl;
                bufferPool.push(receiveBuffer);

                auto sendBuffer = bufferPool.pop();
                sendBuffer->clear();
                sendBuffer->size(512);
                send(sendBuffer);
            }
        });
}

GameClient::~GameClient() {
    io_service.stop();
    thread.join();
}
