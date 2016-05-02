#include "ClientSession.h"
#include "catch.hpp"

TEST_CASE("construction") {
    const boost::asio::ip::udp::endpoint endpoint;
    const uint32_t playerId = 12;
    const float timeStamp = 34.56f;

    ClientSession clientSession(endpoint, playerId, timeStamp);

    REQUIRE(clientSession.getEndpoint() == endpoint);
    REQUIRE(clientSession.getPlayerId() == playerId);
    REQUIRE(clientSession.getLastSeen() == timeStamp);
}

TEST_CASE("update last seen") {
    const boost::asio::ip::udp::endpoint endpoint;
    const uint32_t playerId = 12;
    const float timeStamp = 34.56f;

    ClientSession clientSession(endpoint, playerId, timeStamp);

    REQUIRE(clientSession.getLastSeen() == timeStamp);

    const float lastSeen = 234.42f;
    clientSession.setLastSeen(lastSeen);

    REQUIRE(clientSession.getLastSeen() == lastSeen);
}
