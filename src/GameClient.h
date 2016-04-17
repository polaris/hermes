#ifndef _GameClient_H
#define _GameClient_H

#include "Queue.h"
#include "Buffer.h"

#include <boost/asio.hpp>
#include <thread>

class GameClient {
public:
    GameClient(unsigned int poolSize);

    ~GameClient();

private:
    void initBufferPool();

    void send(const char* address, unsigned short port, Buffer *buffer);

    void receive(Buffer *buffer);

    Queue<Buffer> bufferPool;
    Queue<Buffer> bufferQueue;

    boost::asio::io_service io_service;
    boost::asio::io_service::work work;
    boost::asio::ip::udp::socket socket;
    boost::asio::ip::udp::endpoint senderEndpoint;
    std::thread thread;
};

#endif  // _GameClient_H
