#ifndef _GameServer_H
#define _GameServer_H

#include <boost/asio.hpp>
#include <thread>

class GameServer {
public:
    explicit GameServer(unsigned short port);
    ~GameServer();

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

#endif  // _GameServer_H
