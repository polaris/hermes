#ifndef _PeerRegistry_H
#define _PeerRegistry_H

#include <boost/asio.hpp>

#include <unordered_map>

class PeerRegistry {
public:
    PeerRegistry();

    bool isRegistered(const boost::asio::ip::udp::endpoint& endpoint) const;

    uint32_t getCount() const;

    void add(const boost::asio::ip::udp::endpoint& endpoint);

    void remove(const boost::asio::ip::udp::endpoint& endpoint);

    void reset();

    void forEachPeer(std::function<void (const boost::asio::ip::udp::endpoint&)> fun) const;

private:
    std::unordered_map<std::string, boost::asio::ip::udp::endpoint> peers_;
};

#endif  // _PeerRegistry_H
