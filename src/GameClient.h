#ifndef _GameClient_H
#define _GameClient_H

#include "Queue.h"
#include "Buffer.h"

#include <boost/asio.hpp>
#include <thread>

class GameClient {
public:
    GameClient(const char *address, unsigned short port);
    ~GameClient();

private:
    void send(Buffer *buffer);
    void receive(Buffer *buffer);

    Queue<Buffer> bufferPool;

    boost::asio::io_service io_service;
    boost::asio::io_service::work work;
    boost::asio::ip::udp::socket socket;
    boost::asio::ip::udp::endpoint endpoint;
    std::thread thread;
};

#endif  // _GameClient_H
