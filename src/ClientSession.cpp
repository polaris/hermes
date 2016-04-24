#include "ClientSession.h"
#include "Packet.h"
#include "Clock.h"

#include <boost/log/trivial.hpp>

ClientSession::ClientSession(const boost::asio::ip::udp::endpoint& clientEndpoint, float currenTime)
: clientEndpoint_(clientEndpoint)
, lastSeen_(currenTime) {
}

ClientSession::~ClientSession() {
}

void ClientSession::handleInput(Packet* packet, const Clock& clock) {
    BOOST_LOG_TRIVIAL(debug) << "Received a INPUT from client " << packet->getEndpoint();
    lastSeen_ = clock.getTime();
}

float ClientSession::getLastSeen() const {
    return lastSeen_;
}
