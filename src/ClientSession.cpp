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

void ClientSession::handleInput(Packet*, const Clock& clock) {
    lastSeen_ = clock.getTime();
}

const boost::asio::ip::udp::endpoint ClientSession::getEndpoint() const {
    return clientEndpoint_;
}

float ClientSession::getLastSeen() const {
    return lastSeen_;
}

unsigned int ClientSession::getPlayerId() const {
    return playerId_;
}
