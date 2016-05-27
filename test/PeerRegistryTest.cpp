#include "GamePeer.h"

#include <catch.hpp>

#include <boost/lexical_cast.hpp>

TEST_CASE("Registry is empty after construction") {
    const PeerRegistry peerRegistry;
    REQUIRE(peerRegistry.getCount() == 0);
}

TEST_CASE("Adding peers increases the count") {
    PeerRegistry peerRegistry;

    peerRegistry.add(boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 12345), 1);
    REQUIRE(peerRegistry.getCount() == 1);

    peerRegistry.add(boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string("127.0.0.2"), 12345), 2);
    REQUIRE(peerRegistry.getCount() == 2);

    peerRegistry.add(boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 12346), 3);
    REQUIRE(peerRegistry.getCount() == 3);
}

TEST_CASE("Removing peers decreases the count") {
    const boost::asio::ip::udp::endpoint ep1(boost::asio::ip::address::from_string("127.0.0.1"), 12345);
    const boost::asio::ip::udp::endpoint ep2(boost::asio::ip::address::from_string("127.0.0.2"), 12345);
    const boost::asio::ip::udp::endpoint ep3(boost::asio::ip::address::from_string("127.0.0.1"), 12346);

    PeerRegistry peerRegistry;
    peerRegistry.add(ep1, 1);
    peerRegistry.add(ep2, 2);
    peerRegistry.add(ep3, 3);
    REQUIRE(peerRegistry.getCount() == 3);

    peerRegistry.remove(ep1);
    REQUIRE(peerRegistry.getCount() == 2);
    peerRegistry.remove(ep2);
    REQUIRE(peerRegistry.getCount() == 1);
    peerRegistry.remove(ep3);
    REQUIRE(peerRegistry.getCount() == 0);
}

TEST_CASE("Adding the same peer twice throws an exception") {
    const boost::asio::ip::udp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 12345);

    PeerRegistry peerRegistry;

    peerRegistry.add(ep, 1);
    REQUIRE_THROWS_AS(peerRegistry.add(ep, 2), std::logic_error);
}

TEST_CASE("Check if peer is registered") {
    const boost::asio::ip::udp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 12345);

    PeerRegistry peerRegistry;

    peerRegistry.add(ep, 1);
    REQUIRE(peerRegistry.isRegistered(ep) == true);

    peerRegistry.remove(ep);
    REQUIRE(peerRegistry.isRegistered(ep) == false);
}

TEST_CASE("Reseting the registry removes all registered peers") {
    PeerRegistry peerRegistry;

    peerRegistry.add(boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 12345), 1);
    peerRegistry.add(boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string("127.0.0.2"), 12345), 2);
    peerRegistry.add(boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 12346), 3);

    REQUIRE(peerRegistry.getCount() == 3);

    peerRegistry.reset();

    REQUIRE(peerRegistry.getCount() == 0);
}

TEST_CASE("Iterate over all peers in the registry") {
    PeerRegistry peerRegistry;

    std::map<std::string, std::pair<unsigned short, uint32_t>> ipAddresses = {
        { "127.0.0.1",     { 1001, 1 } },
        { "10.149.1.123",  { 1002, 2 } },
        { "10.149.2.123",  { 1003, 3 } },
        { "192.168.2.105", { 1004, 4 } }
    };

    unsigned int expectedCount = 0;
    for (const auto& pair : ipAddresses) {
        peerRegistry.add(boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(pair.first), pair.second.first), pair.second.second);
        expectedCount += 1;
    }

    unsigned int actualCount = 0;
    peerRegistry.forEachPeer([&actualCount, &ipAddresses] (const Peer& peer) {
        REQUIRE(ipAddresses.size() > actualCount);
        const auto s = boost::lexical_cast<std::string>(peer.endpoint.address());
        REQUIRE(ipAddresses[s].first == peer.endpoint.port());
        REQUIRE(ipAddresses[s].second == peer.playerId);
        actualCount += 1;
    });
    REQUIRE(expectedCount == actualCount);
}

TEST_CASE("Registered peers can be verified by checking player ID and endpoint") {
    PeerRegistry peerRegistry;

    const auto ep1 = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 12345);
    const auto ep2 = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string("127.0.0.2"), 12346);
    const auto ep3 = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string("127.0.0.3"), 12347);

    peerRegistry.add(ep1, 1);
    peerRegistry.add(ep2, 2);
    peerRegistry.add(ep3, 3);

    REQUIRE(peerRegistry.verifyPeer(1, ep1) == true);
    REQUIRE(peerRegistry.verifyPeer(2, ep2) == true);
    REQUIRE(peerRegistry.verifyPeer(3, ep3) == true);

    REQUIRE(peerRegistry.verifyPeer(3, ep1) == false);
    REQUIRE(peerRegistry.verifyPeer(1, ep2) == false);
    REQUIRE(peerRegistry.verifyPeer(2, ep3) == false);

    REQUIRE(peerRegistry.verifyPeer(2, ep1) == false);
    REQUIRE(peerRegistry.verifyPeer(3, ep2) == false);
    REQUIRE(peerRegistry.verifyPeer(1, ep3) == false);
}
