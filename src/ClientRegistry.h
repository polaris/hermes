#ifndef _ClientRegistry_H
#define _ClientRegistry_H

#include "ClientSession.h"

#include <boost/asio.hpp>

#include <unordered_map>
#include <memory>

class ClientRegistry {
public:
    ClientRegistry();

    ClientSession* addClientSession(unsigned int playerId, const boost::asio::ip::udp::endpoint& endpoint, float timeStamp);

    ClientSession* getClientSession(unsigned int playerId);

    void removeClientSession(unsigned int playerId);

    bool verifyClientSession(unsigned int playerId, const boost::asio::ip::udp::endpoint& endpoint);

    bool hasClientSession(const boost::asio::ip::udp::endpoint& endpoint);

    void checkForDisconnects(float currentTime);

private:
    std::unordered_map<unsigned int, std::unique_ptr<ClientSession>> clientSessions_;
    std::unordered_map<std::string, unsigned int> playerIds_;
};

#endif  // _ClientRegistry_H
