#include "ClientSession.h"
#include "Packet.h"
#include "Clock.h"

#include <boost/log/trivial.hpp>

ClientSession::ClientSession(const boost::asio::ip::udp::endpoint& clientEndpoint, unsigned int playerId, float currenTime)
: clientEndpoint_(clientEndpoint)
, playerId_(playerId)
, lastSeen_(currenTime) {
}

ClientSession::~ClientSession() {
}

const boost::asio::ip::udp::endpoint ClientSession::getEndpoint() const {
    return clientEndpoint_;
}

unsigned int ClientSession::getPlayerId() const {
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
