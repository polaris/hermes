#ifndef _Transceiver_H
#define _Transceiver_H

#include "Queue.h"
#include "Packet.h"

#include <boost/asio.hpp>
#include <thread>

class Transceiver {
public:
    Transceiver(unsigned short port, Queue<Packet>& packetPool, Queue<Packet>& incomingPackets);

    Transceiver(Queue<Packet>& packetPool, Queue<Packet>& incomingPackets);

    ~Transceiver();

    void sendTo(Packet* packet);

private:
    void receiveFrom(Packet* packet);

    Queue<Packet>& packetPool_;
    Queue<Packet>& incomingPackets_;

    boost::asio::io_service io_service_;
    boost::asio::io_service::work work_;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint senderEndpoint_;
    std::thread thread_;
};

#endif  // _Transceiver_H
