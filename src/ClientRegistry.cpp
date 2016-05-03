#include "ClientRegistry.h"
#include "Protocol.h"

#include <boost/lexical_cast.hpp>

#include <vector>

ClientRegistry::ClientRegistry()
: clientSessions_()
, playerIds_() {
}

ClientSession* ClientRegistry::addClientSession(uint32_t playerId, const boost::asio::ip::udp::endpoint& endpoint, float timeStamp) {
    auto newClientSession = std::make_unique<ClientSession>(endpoint, playerId, timeStamp);
    clientSessions_.insert(std::make_pair(playerId, std::move(newClientSession)));
    playerIds_.insert(std::make_pair(boost::lexical_cast<std::string>(endpoint), playerId));
    return clientSessions_[playerId].get();
}

ClientSession* ClientRegistry::getClientSession(uint32_t playerId) {
    const auto itr = clientSessions_.find(playerId);
    if (itr != clientSessions_.end()) {
        return (*itr).second.get();
    }
    return nullptr;
}

void ClientRegistry::removeClientSession(uint32_t playerId) {
    const auto itr = clientSessions_.find(playerId);
    if (itr != clientSessions_.end()) {
        playerIds_.erase(playerIds_.find(boost::lexical_cast<std::string>(itr->second->getEndpoint())));
        clientSessions_.erase(itr);
    }
}

bool ClientRegistry::verifyClientSession(uint32_t playerId, const boost::asio::ip::udp::endpoint& endpoint) {
    if (clientSessions_.find(playerId) != clientSessions_.end()) {
        const auto s = boost::lexical_cast<std::string>(endpoint);
        return playerIds_[s] == playerId;
    }
    return false;
}

bool ClientRegistry::hasClientSession(const boost::asio::ip::udp::endpoint& endpoint) {
    return playerIds_.find(boost::lexical_cast<std::string>(endpoint)) != playerIds_.end();
}

void ClientRegistry::checkForDisconnects(float currentTime, std::function<void (uint32_t)> fun) {
    std::vector<uint32_t> clientsToBeRemoved;
    for (const auto& clientSession : clientSessions_) {
        if ((currentTime - clientSession.second->getLastSeen()) > PROTOCOL_CLIENT_TIMEOUT) {
            clientsToBeRemoved.push_back(clientSession.first);
        }
    }
    for (const auto& playerId : clientsToBeRemoved) {
        removeClientSession(playerId);
        fun(playerId);
    }
}

void ClientRegistry::forEachClientSession(std::function<void (ClientSession*)> fun) {
    for (auto& pair : clientSessions_) {
        fun(pair.second.get());
    }
}
