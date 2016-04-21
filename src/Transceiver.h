#ifndef _Transceiver_H
#define _Transceiver_H

#include "Queue.h"
#include "Buffer.h"

#include <boost/asio.hpp>
#include <thread>

class Transceiver {
public:
    Transceiver(unsigned short port, Queue<Buffer>& bufferPool, Queue<Buffer>& incomingPackets);

    Transceiver(Queue<Buffer>& bufferPool, Queue<Buffer>& incomingPackets);

    ~Transceiver();

    void sendTo(Buffer* buffer);

private:
    void receiveFrom(Buffer* buffer);

    Queue<Buffer>& bufferPool_;
    Queue<Buffer>& incomingPackets_;

    boost::asio::io_service io_service_;
    boost::asio::io_service::work work_;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint senderEndpoint_;
    std::thread thread_;
};

#endif  // _Transceiver_H
