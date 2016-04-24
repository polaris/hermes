#ifndef _ClientSession_H
#define _ClientSession_H

#include <boost/asio.hpp>

class Clock;
class Packet;

class ClientSession {
public:
    ClientSession(const boost::asio::ip::udp::endpoint& clientEndpoint, unsigned int playerId, float currenTime);

    ~ClientSession();

    void handleInput(Packet* packet, const Clock& clock);

    const boost::asio::ip::udp::endpoint getEndpoint() const;

    float getLastSeen() const;

    unsigned int getPlayerId() const;

private:
    const boost::asio::ip::udp::endpoint clientEndpoint_;

    const unsigned int playerId_;

    float lastSeen_;
};

#endif  // _ClientSession_H
