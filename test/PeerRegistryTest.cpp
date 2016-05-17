#include "GamePeer.h"
#include "catch.hpp"

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
