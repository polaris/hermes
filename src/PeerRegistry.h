#ifndef _PeerRegistry_H
#define _PeerRegistry_H

#include <boost/asio.hpp>

#include <unordered_map>

using Peer = std::pair<boost::asio::ip::udp::endpoint, uint32_t>;

class PeerRegistry {
public:
    PeerRegistry();

    bool isRegistered(const boost::asio::ip::udp::endpoint& endpoint) const;

    uint32_t getCount() const;

    void add(const boost::asio::ip::udp::endpoint& endpoint, uint32_t playerId);

    void remove(const boost::asio::ip::udp::endpoint& endpoint);

    void reset();

    void forEachPeer(std::function<void (const Peer&)> fun) const;

private:
    std::unordered_map<std::string, Peer> peers_;
};

#endif  // _PeerRegistry_H
