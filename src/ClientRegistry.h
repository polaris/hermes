#ifndef _ClientRegistry_H
#define _ClientRegistry_H

#include "ClientSession.h"

#include <boost/asio.hpp>

#include <unordered_map>
#include <memory>
#include <functional>

/** Manages client session.
 */
class ClientRegistry {
public:
    /** Constructor
     */
    ClientRegistry();

    /** Adds a session to the registry.

        \param playerId the ID of the new player
        \param endpoint the endpoint of the new player.
        \param timeStamp the timestamp when this player was seen.
        \return the newly created client session.
     */
    ClientSession* addClientSession(uint32_t playerId, const boost::asio::ip::udp::endpoint& endpoint, float timeStamp);

    /** Get the client session belonging to a given player.

        \param playerId the ID of the player that belongs to the client session in question.
        \return a pointer to the client session if available, otherwise nullptr.
     */
    ClientSession* getClientSession(uint32_t playerId);

    /** Removes a client session that belongs to a given player.

        \param playerId the ID of the player that belongs to the client session in question.
     */
    void removeClientSession(uint32_t playerId);

    /** Verifies that a given player ID matches to a given endpoint.

        \param playerId a player ID.
        \param endpoint an endpoint.
        \return true if the endpoint belongs to the player with the given ID, otherwise false.
     */
    bool verifyClientSession(uint32_t playerId, const boost::asio::ip::udp::endpoint& endpoint);

    /** Checks if there is a client session for a given endpoint.

        \param endpoint the endpoint to check for.
        \return true if there is a client session for the given endpoint, otherwise false.
     */
    bool hasClientSession(const boost::asio::ip::udp::endpoint& endpoint);

    /** Checks all client session for connectivity and removes disconnected clients.

        \param currentTime the current time.
        \param fun a callback function to be called for each removed client.
     */
    void checkForDisconnects(float currentTime, std::function<void (uint32_t)> fun);

    /** Iterates over all client sessions and calls a given callback function for each session.

        \param fun a callback function.
     */
    void forEachClientSession(std::function<void (ClientSession*)> fun);

private:
    std::unordered_map<uint32_t, std::unique_ptr<ClientSession>> clientSessions_;
    std::unordered_map<std::string, uint32_t> playerIds_;
};

#endif  // _ClientRegistry_H
