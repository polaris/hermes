#ifndef _ClientSession_H
#define _ClientSession_H

#include "MoveList.h"

#include <boost/asio.hpp>

class ClientSession {
public:
    ClientSession(const boost::asio::ip::udp::endpoint& clientEndpoint, uint32_t playerId, float currenTime);

    const boost::asio::ip::udp::endpoint getEndpoint() const;

    uint32_t getPlayerId() const;

    MoveList& getMoveList();

    void setLastSeen(float timeStamp);

    float getLastSeen() const;

    void setLatestInputTime(float timeStamp);

    float getLatestInputTime() const;

private:
    const boost::asio::ip::udp::endpoint clientEndpoint_;

    const uint32_t playerId_;

    MoveList moveList_;

    float lastSeen_;

    float latestInputTime_;
};

#endif  // _ClientSession_H
