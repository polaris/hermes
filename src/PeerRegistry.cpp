#include "PeerRegistry.h"

#include <boost/lexical_cast.hpp>

#include <stdexcept>

PeerRegistry::PeerRegistry()
: peers_() {
}

bool PeerRegistry::isRegistered(const boost::asio::ip::udp::endpoint& endpoint) const {
    const auto s = boost::lexical_cast<std::string>(endpoint);
    return peers_.find(s) != peers_.end();
}

uint32_t PeerRegistry::getCount() const {
    return static_cast<uint32_t>(peers_.size());
}

void PeerRegistry::add(const boost::asio::ip::udp::endpoint& endpoint, uint32_t playerId) {
    const auto s = boost::lexical_cast<std::string>(endpoint);
    if (peers_.find(s) != peers_.end()) {
        throw std::logic_error("peer already registered");
    }    
    peers_.insert(std::make_pair(s, std::make_pair(endpoint, playerId)));
}

void PeerRegistry::remove(const boost::asio::ip::udp::endpoint& endpoint) {
    const auto s = boost::lexical_cast<std::string>(endpoint);
    peers_.erase(s);
}

void PeerRegistry::reset() {
    peers_.erase(peers_.begin(), peers_.end());
}

void PeerRegistry::forEachPeer(std::function<void (const Peer&)> fun) const {
    for (const auto& p : peers_) {
        fun(p.second);
    }
}
