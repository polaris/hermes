#include "ClientRegistry.h"
#include "Protocol.h"
#include "catch.hpp"

using namespace boost::asio::ip;

TEST_CASE("registry is empty after construction") {
    ClientRegistry clientRegistry;

    bool noClientSessions = true;
    clientRegistry.forEachClientSession([&noClientSessions] (ClientSession*) {
        noClientSessions = false;
    });

    REQUIRE(noClientSessions);
}

TEST_CASE("registry is not empty after adding a session") {
    ClientRegistry clientRegistry;

    const uint32_t playerId = 123;
    const udp::endpoint endpoint(address::from_string("127.0.0.2"), 32132);
    const float timeStamp = 987.65f;

    clientRegistry.addClientSession(playerId, endpoint, timeStamp);

    bool noClientSessions = true;
    clientRegistry.forEachClientSession([&] (ClientSession* session) {
        noClientSessions = false;
        REQUIRE(session->getPlayerId() == playerId);
        REQUIRE(session->getEndpoint() == endpoint);
        REQUIRE(session->getLastSeen() == timeStamp);
    });

    REQUIRE(noClientSessions == false);
}

TEST_CASE("loop over all session") {
    ClientRegistry clientRegistry;

    const uint32_t playerId = 123;
    const udp::endpoint endpoint(address::from_string("127.0.0.2"), 32132);
    const float timeStamp = 987.65f;

    clientRegistry.addClientSession(123, udp::endpoint(address::from_string("127.0.0.1"), 32132), 1.0f);
    clientRegistry.addClientSession(124, udp::endpoint(address::from_string("127.0.0.2"), 32132), 2.0f);
    clientRegistry.addClientSession(125, udp::endpoint(address::from_string("127.0.0.3"), 32132), 3.0f);
    clientRegistry.addClientSession(126, udp::endpoint(address::from_string("127.0.0.4"), 32132), 4.0f);
    clientRegistry.addClientSession(127, udp::endpoint(address::from_string("127.0.0.5"), 32132), 5.0f);

    unsigned int count = 0;
    clientRegistry.forEachClientSession([&count] (ClientSession*) {
        count += 1;
    });

    REQUIRE(count == 5);
}

TEST_CASE("add sessions to the registry") {
    ClientRegistry clientRegistry;

    const uint32_t playerId = 123;
    const udp::endpoint endpoint(address::from_string("127.0.0.2"), 32132);
    const float timeStamp = 987.65f;

    const auto session = clientRegistry.addClientSession(playerId, endpoint, timeStamp);

    REQUIRE(session->getPlayerId() == playerId);
    REQUIRE(session->getEndpoint() == endpoint);
    REQUIRE(session->getLastSeen() == timeStamp);
}

TEST_CASE("get an non-existing sessions from the registry") {
    ClientRegistry clientRegistry;

    REQUIRE(clientRegistry.getClientSession(0) == nullptr);
}

TEST_CASE("get an existing sessions from the registry") {
    ClientRegistry clientRegistry;

    const uint32_t playerId = 123;
    const udp::endpoint endpoint(address::from_string("127.0.0.2"), 32132);
    const float timeStamp = 987.65f;

    clientRegistry.addClientSession(playerId, endpoint, timeStamp);

    const auto session = clientRegistry.getClientSession(playerId);

    REQUIRE(session->getPlayerId() == playerId);
    REQUIRE(session->getEndpoint() == endpoint);
    REQUIRE(session->getLastSeen() == timeStamp);
}

TEST_CASE("remove an existing sessions from the registry") {
    ClientRegistry clientRegistry;

    const uint32_t playerId = 123;
    const udp::endpoint endpoint(address::from_string("127.0.0.2"), 32132);
    const float timeStamp = 987.65f;

    clientRegistry.addClientSession(playerId, endpoint, timeStamp);

    auto session = clientRegistry.getClientSession(playerId);
    REQUIRE(session != nullptr);

    clientRegistry.removeClientSession(playerId);

    session = clientRegistry.getClientSession(playerId);
    REQUIRE(session == nullptr);
}

TEST_CASE("verify a session (match)") {
    ClientRegistry clientRegistry;

    const uint32_t playerId1 = 123;
    const uint32_t playerId2 = 124;
    const udp::endpoint endpoint1(address::from_string("127.0.0.2"), 32132);
    const udp::endpoint endpoint2(address::from_string("127.0.0.3"), 32132);
    const float timeStamp = 987.65f;

    clientRegistry.addClientSession(playerId1, endpoint1, timeStamp);
    clientRegistry.addClientSession(playerId2, endpoint2, timeStamp);

    REQUIRE(clientRegistry.verifyClientSession(playerId1, endpoint1));
    REQUIRE(clientRegistry.verifyClientSession(playerId2, endpoint2));
}

TEST_CASE("verify a session (non-match)") {
    ClientRegistry clientRegistry;

    const uint32_t playerId1 = 123;
    const uint32_t playerId2 = 124;
    const udp::endpoint endpoint1(address::from_string("127.0.0.2"), 32132);
    const udp::endpoint endpoint2(address::from_string("127.0.0.3"), 32132);
    const float timeStamp = 987.65f;

    clientRegistry.addClientSession(playerId1, endpoint1, timeStamp);
    clientRegistry.addClientSession(playerId2, endpoint2, timeStamp);

    REQUIRE(!clientRegistry.verifyClientSession(playerId2, endpoint1));
    REQUIRE(!clientRegistry.verifyClientSession(playerId1, endpoint2));
}

TEST_CASE("check for an existing session") {
    ClientRegistry clientRegistry;

    const uint32_t playerId1 = 123;
    const uint32_t playerId2 = 124;
    const udp::endpoint endpoint1(address::from_string("127.0.0.2"), 32132);
    const udp::endpoint endpoint2(address::from_string("127.0.0.3"), 32132);
    const float timeStamp = 987.65f;

    clientRegistry.addClientSession(playerId1, endpoint1, timeStamp);
    clientRegistry.addClientSession(playerId2, endpoint2, timeStamp);

    REQUIRE(clientRegistry.hasClientSession(endpoint1));
    REQUIRE(clientRegistry.hasClientSession(endpoint2));
}

TEST_CASE("check for an non-existing session") {
    ClientRegistry clientRegistry;

    const uint32_t playerId1 = 123;
    const uint32_t playerId2 = 124;
    const udp::endpoint endpoint1(address::from_string("127.0.0.2"), 32132);
    const udp::endpoint endpoint2(address::from_string("127.0.0.3"), 32132);
    const udp::endpoint endpoint3(address::from_string("127.0.0.2"), 32133);
    const udp::endpoint endpoint4(address::from_string("127.0.0.4"), 32132);
    const float timeStamp = 987.65f;

    clientRegistry.addClientSession(playerId1, endpoint1, timeStamp);
    clientRegistry.addClientSession(playerId2, endpoint2, timeStamp);

    REQUIRE(!clientRegistry.hasClientSession(endpoint3));
    REQUIRE(!clientRegistry.hasClientSession(endpoint4));
}

TEST_CASE("check for disconnects: in time") {
    ClientRegistry clientRegistry;

    const uint32_t playerId = 123;
    const udp::endpoint endpoint(address::from_string("127.0.0.2"), 32132);
    const float timeStamp = 123.46f;

    clientRegistry.addClientSession(playerId, endpoint, timeStamp);

    bool noDisconnects = true;
    clientRegistry.checkForDisconnects(timeStamp + PROTOCOL_CLIENT_TIMEOUT, [&noDisconnects] (uint32_t) {
        noDisconnects = false;
    });

    REQUIRE(noDisconnects);
}

TEST_CASE("check for disconnects: out of time") {
    ClientRegistry clientRegistry;

    const uint32_t playerId = 123;
    const udp::endpoint endpoint(address::from_string("127.0.0.2"), 32132);
    const float timeStamp = 123.46f;

    clientRegistry.addClientSession(playerId, endpoint, timeStamp);

    uint32_t disconnectedClient = 0;
    clientRegistry.checkForDisconnects(timeStamp + PROTOCOL_CLIENT_TIMEOUT + 1, [&disconnectedClient] (uint32_t pid) {
        disconnectedClient = pid;
    });

    REQUIRE(disconnectedClient == playerId);
}
