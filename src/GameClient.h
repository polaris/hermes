#ifndef _GameClient_H
#define _GameClient_H

#include <boost/asio.hpp>

#include <thread>
#include <iostream>

class GameClient {
public:
    GameClient(const char *address, unsigned short port);
    ~GameClient();

private:
    void send();
    void receive();

    boost::asio::io_service io_service;
    boost::asio::io_service::work work;
    boost::asio::ip::udp::socket socket;
    boost::asio::ip::udp::endpoint endpoint;
    std::thread thread;
    enum { MaxLength = 1024 };
    char data[MaxLength];
};

GameClient::GameClient(const char *address, unsigned short port)
: io_service()
, work(io_service)
, socket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0))
, endpoint(boost::asio::ip::address::from_string(address), port)
, thread([this] () { io_service.run(); }) {
    send();
}

void GameClient::send() {
    socket.async_send_to(boost::asio::buffer(data, MaxLength), endpoint,
        [this] (const boost::system::error_code &ec, std::size_t bytesTransferred) {
            if (ec) {
                std::cerr << ec.message() << std::endl;
            } else {
                std::cout << bytesTransferred << " bytes transferred" << std::endl;
                receive();
            }
        });
}

void GameClient::receive() {
    socket.async_receive_from(boost::asio::buffer(data, MaxLength), endpoint,
        [this] (const boost::system::error_code &ec, std::size_t bytesReceived) {
            if (ec) {
                std::cerr << ec.message() << std::endl;
            } else {
                std::cout << bytesReceived << " bytes received" << std::endl;
                send();
            }
        });
}

GameClient::~GameClient() {
    io_service.stop();
    thread.join();
}

#endif  // _GameClient_H
