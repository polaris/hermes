#include "ClientRegistry.h"
#include "Protocol.h"

#include <boost/lexical_cast.hpp>
#include <boost/log/trivial.hpp>

#include <vector>

ClientRegistry::ClientRegistry() {
}

ClientSession* ClientRegistry::addClientSession(unsigned int playerId, const boost::asio::ip::udp::endpoint& endpoint, float timeStamp) {
    auto newClientSession = std::make_unique<ClientSession>(endpoint, playerId, timeStamp);
    clientSessions_.insert(std::make_pair(playerId, std::move(newClientSession)));
    playerIds_.insert(std::make_pair(boost::lexical_cast<std::string>(endpoint), playerId));
    return clientSessions_[playerId].get();
}

ClientSession* ClientRegistry::getClientSession(unsigned int playerId) {
    auto itr = clientSessions_.find(playerId);
    if (itr != clientSessions_.end()) {
        return (*itr).second.get();
    }
    return nullptr;
}

void ClientRegistry::removeClientSession(unsigned int playerId) {
    auto itr = clientSessions_.find(playerId);
    if (itr != clientSessions_.end()) {
        playerIds_.erase(playerIds_.find(boost::lexical_cast<std::string>(itr->second->getEndpoint())));
        clientSessions_.erase(itr);
    }
}

bool ClientRegistry::verifyClientSession(unsigned int playerId, const boost::asio::ip::udp::endpoint& endpoint) {
    if (clientSessions_.find(playerId) != clientSessions_.end()) {
        const auto s = boost::lexical_cast<std::string>(endpoint);
        return playerIds_[s] == playerId;
    }
    return false;
}

bool ClientRegistry::hasClientSession(const boost::asio::ip::udp::endpoint& endpoint) {
    return playerIds_.find(boost::lexical_cast<std::string>(endpoint)) != playerIds_.end();
}

void ClientRegistry::checkForDisconnects(float currentTime) {
    std::vector<unsigned int> clientsToBeRemoved;
    for (const auto& clientSession : clientSessions_) {
        if ((currentTime - clientSession.second->getLastSeen()) > PROTOCOL_CLIENT_TIMEOUT) {
            clientsToBeRemoved.push_back(clientSession.first);
        }
    }
    for (const auto& playerId : clientsToBeRemoved) {
        BOOST_LOG_TRIVIAL(debug) << "Remove disconnected client " << playerId;
        removeClientSession(playerId);
    }
}

void ClientRegistry::forEachSession(std::function<void (ClientSession*)> fun) {
    for (auto& pair : clientSessions_) {
        fun(pair.second.get());
    }
}
