#ifndef _GameServer_H
#define _GameServer_H

#include "Game.h"
#include "Queue.h"
#include "Packet.h"
#include "ClientSession.h"
#include "Transceiver.h"

#include <unordered_map>
#include <memory>

class Renderer;
class Clock;

using PlayerIdToSessionMap = std::unordered_map<unsigned int, std::unique_ptr<ClientSession>>;
using EndpointToPlayerIdMap = std::unordered_map<std::string, unsigned int>;

class GameServer : public Game {
public:
    GameServer(unsigned int frameRate, unsigned short port, Renderer& renderer);

private:
    void handleWillUpdateWorld(const Clock& clock) override;
    void handleDidUpdateWorld(const Clock& clock) override;
    void handleEvent(SDL_Event &event, bool& running) override;

    void processIncomingPackets(const Clock& clock);
    void handlePacket(Packet* packet, const Clock& clock);
    void handleHello(Packet* packet, const Clock& clock);
    void handleInput(Packet* packet, const Clock& clock);
    void handleGoodBye(Packet* packet);

    void checkForDisconnects(const Clock& clock);

    void renderWorld();
    void sendOutgoingPackets();

    unsigned int nextPlayerId_;
    unsigned int nextObjectId_;

    PlayerIdToSessionMap clientSessions_;
    EndpointToPlayerIdMap playerIds_;
    void createNewClientSession(unsigned int playerId, const boost::asio::ip::udp::endpoint& endpoint, const Clock& clock);
    bool verifyClientSession(unsigned int playerId, const boost::asio::ip::udp::endpoint& endpoint);
    bool hasClientSession(const boost::asio::ip::udp::endpoint& endpoint);
    void removeClientSession(unsigned int playerId);

    Queue<Packet> packetPool_;
    Queue<Packet> incomingPackets_;
    Transceiver transceiver_;
};

#endif  // _GameServer_H
