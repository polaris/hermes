#ifndef _Transceiver_H
#define _Transceiver_H

#include <boost/asio.hpp>
#include <thread>

class Packet;
class PacketSink;

class Transceiver {
public:
    Transceiver(uint16_t port, PacketSink& packetSink);

    explicit Transceiver(PacketSink& packetSink);

    ~Transceiver();

    void sendTo(Packet* packet);

private:
    void receiveFrom(Packet* packet);

    PacketSink& packetSink_;

    boost::asio::io_service io_service_;
    boost::asio::io_service::work work_;
    boost::asio::ip::udp::socket socket_;
    std::thread thread_;
};

#endif  // _Transceiver_H
