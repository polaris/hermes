#ifndef _ClientSession_H
#define _ClientSession_H

#include <boost/asio.hpp>

class Clock;
class Packet;

class ClientSession {
public:
    ClientSession(const boost::asio::ip::udp::endpoint& clientEndpoint, float currenTime);

    ~ClientSession();

    void handleInput(Packet* packet, const Clock& clock);

    float getLastSeen() const;

private:
    const boost::asio::ip::udp::endpoint clientEndpoint_;

    float lastSeen_;
};

#endif  // _ClientSession_H
