#ifndef _PeerRegistry_H
#define _PeerRegistry_H

#include "LatencyEstimator.h"

#include <boost/asio.hpp>

#include <unordered_map>

struct Peer {
    Peer(const boost::asio::ip::udp::endpoint& endpoint_, uint32_t playerId_)
    : endpoint(endpoint_)
    , playerId(playerId_)
    , latencyEstimator(10) {
    }

    const boost::asio::ip::udp::endpoint endpoint;
    const uint32_t playerId;
    LatencyEstimator latencyEstimator;
};

class PeerRegistry {
public:
    PeerRegistry();

    bool isRegistered(const boost::asio::ip::udp::endpoint& endpoint) const;

    bool verifyPeer(uint32_t playerId, const boost::asio::ip::udp::endpoint& endpoint) const;

    uint32_t getCount() const;

    void add(const boost::asio::ip::udp::endpoint& endpoint, uint32_t playerId);

    void remove(const boost::asio::ip::udp::endpoint& endpoint);

    void reset();

    void forEachPeer(std::function<void (const Peer&)> fun) const;

    void addRoundTripTime(const boost::asio::ip::udp::endpoint& endpoint, float roundTripTime);

    const LatencyEstimator& getLatencyEstimator(const boost::asio::ip::udp::endpoint& endpoint) const;

private:
    std::unordered_map<std::string, Peer> peers_;
};

#endif  // _PeerRegistry_H
