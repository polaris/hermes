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

using ClientSessionMap = std::unordered_map<std::string, std::unique_ptr<ClientSession>>;

class GameServer : public Game {
public:
    GameServer(unsigned int frameRate, unsigned short port, Renderer& renderer);

private:
    void handleWillUpdateWorld(const Clock& clock) override;
    void handleDidUpdateWorld(const Clock& clock) override;

    void processIncomingPackets(const Clock& clock);
    void handlePacket(Packet* packet, const Clock& clock);
    void handleHello(Packet* packet, const Clock& clock);
    void handleInput(Packet* packet, const Clock& clock);
    void handleGoodBye(Packet* packet);

    void checkForDisconnects(const Clock& clock);

    ClientSessionMap clientSessions_;
    void createNewClientSession(const boost::asio::ip::udp::endpoint& endpoint, const Clock& clock);
    bool hasClientSession(const boost::asio::ip::udp::endpoint& endpoint);
    void removeClientSession(const boost::asio::ip::udp::endpoint& endpoint);

    Queue<Packet> packetPool_;
    Queue<Packet> incomingPackets_;
    Transceiver transceiver_;
};

#endif  // _GameServer_H
