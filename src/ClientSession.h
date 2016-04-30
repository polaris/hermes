#ifndef _ClientSession_H
#define _ClientSession_H

#include "MoveList.h"

#include <boost/asio.hpp>

class ClientSession {
public:
    ClientSession(const boost::asio::ip::udp::endpoint& clientEndpoint, uint32_t playerId, float currenTime);

    ~ClientSession();

    const boost::asio::ip::udp::endpoint getEndpoint() const;

    uint32_t getPlayerId() const;

    MoveList& getMoveList();

    void setLastSeen(float timeStamp);

    float getLastSeen() const;

private:
    const boost::asio::ip::udp::endpoint clientEndpoint_;

    const uint32_t playerId_;

    MoveList moveList_;

    float lastSeen_;
};

#endif  // _ClientSession_H
