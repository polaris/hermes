#include <boost/asio.hpp>

#include <thread>
#include <iostream>

class GameServer {
public:
    explicit GameServer(unsigned short port);
    ~GameServer();

private:
    void receive();

    boost::asio::io_service io_service;
    boost::asio::io_service::work work;
    boost::asio::ip::udp::socket socket;
    boost::asio::ip::udp::endpoint senderEndpoint;
    std::thread thread;
    enum { MaxLength = 1024 };
    char data[MaxLength];
};

GameServer::GameServer(unsigned short port)
: io_service()
, work(io_service)
, socket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port))
, thread([this] () { io_service.run(); }) {
    receive();
}

void GameServer::receive() {
    socket.async_receive_from(boost::asio::buffer(data, MaxLength), senderEndpoint,
        [this] (const boost::system::error_code &ec, std::size_t bytesReceived) {
            if (ec) {
                std::cerr << ec.message() << std::endl;
            } else {
                std::cout << bytesReceived << " bytes received" << std::endl;
                receive();
            }
        });
}

GameServer::~GameServer() {
    io_service.stop();
    thread.join();
}

int main() {
    GameServer gameServer(12345);
    std::cin.ignore();
}
