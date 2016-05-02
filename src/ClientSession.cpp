#include "ClientSession.h"
#include "Packet.h"
#include "Clock.h"

#include <boost/log/trivial.hpp>

ClientSession::ClientSession(const boost::asio::ip::udp::endpoint& clientEndpoint, uint32_t playerId, float currenTime)
: clientEndpoint_(clientEndpoint)
, playerId_(playerId)
, moveList_()
, lastSeen_(currenTime) {
}

const boost::asio::ip::udp::endpoint ClientSession::getEndpoint() const {
    return clientEndpoint_;
}

uint32_t ClientSession::getPlayerId() const {
    return playerId_;
}

MoveList& ClientSession::getMoveList() {
    return moveList_;
}

void ClientSession::setLastSeen(float timeStamp) {
    lastSeen_ = timeStamp;
}

float ClientSession::getLastSeen() const {
    return lastSeen_;
}
