#ifndef _ClientRegistry_H
#define _ClientRegistry_H

#include "ClientSession.h"

#include <boost/asio.hpp>

#include <unordered_map>
#include <memory>
#include <functional>

class ClientRegistry {
public:
    ClientRegistry();

    ClientSession* addClientSession(uint32_t playerId, const boost::asio::ip::udp::endpoint& endpoint, float timeStamp);

    ClientSession* getClientSession(uint32_t playerId);

    void removeClientSession(uint32_t playerId);

    bool verifyClientSession(uint32_t playerId, const boost::asio::ip::udp::endpoint& endpoint);

    bool hasClientSession(const boost::asio::ip::udp::endpoint& endpoint);

    void checkForDisconnects(float currentTime, std::function<void (uint32_t)> fun);

    void forEachClientSession(std::function<void (ClientSession*)> fun);

private:
    std::unordered_map<uint32_t, std::unique_ptr<ClientSession>> clientSessions_;
    std::unordered_map<std::string, uint32_t> playerIds_;
};

#endif  // _ClientRegistry_H
