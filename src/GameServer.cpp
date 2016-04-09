#include "GameServer.h"

GameServer::GameServer(unsigned short port)
: io_service()
, work(io_service)
, socket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port))
, thread([this] () { io_service.run(); }) {
    receive();
}

void GameServer::receive() {
    socket.async_receive_from(boost::asio::buffer(data, MaxLength/2), senderEndpoint,
        [this] (const boost::system::error_code &ec, std::size_t bytesReceived) {
            if (ec) {
                std::cerr << ec.message() << std::endl;
            } else {
                std::cout << bytesReceived << " bytes received" << std::endl;
                send(senderEndpoint);
            }
        });
}

void GameServer::send(const boost::asio::ip::udp::endpoint &endpoint) {
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

GameServer::~GameServer() {
    io_service.stop();
    thread.join();
}
