#include <boost/asio.hpp>

#include <thread>
#include <iostream>

class Server {
public:
    explicit Server(unsigned short port);
    ~Server();

private:
    void receive();
    void send(const boost::asio::ip::udp::endpoint &endpoint);

    boost::asio::io_service io_service;
    boost::asio::io_service::work work;
    boost::asio::ip::udp::socket socket;
    boost::asio::ip::udp::endpoint senderEndpoint;
    std::thread thread;
    enum { MaxLength = 1024 };
    char data[MaxLength];
};

Server::Server(unsigned short port)
: io_service()
, work(io_service)
, socket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port))
, thread([this] () { io_service.run(); }) {
    receive();
}

void Server::receive() {
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

void Server::send(const boost::asio::ip::udp::endpoint &endpoint) {
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

Server::~Server() {
    io_service.stop();
    thread.join();
}

int main() {
    Server Server(12345);
    std::cin.ignore();
}
