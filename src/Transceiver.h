#ifndef _Transceiver_H
#define _Transceiver_H

#include "BufferedQueue.h"

#include <boost/asio.hpp>
#include <thread>

class Packet;

class Transceiver {
public:
    Transceiver(uint16_t port, BufferedQueue& bufferedQueue);

    Transceiver(BufferedQueue& bufferedQueue);

    ~Transceiver();

    void sendTo(Packet* packet);

private:
    void receiveFrom(Packet* packet);

    BufferedQueue& bufferedQueue_;

    boost::asio::io_service io_service_;
    boost::asio::io_service::work work_;
    boost::asio::ip::udp::socket socket_;
    std::thread thread_;
};

#endif  // _Transceiver_H
