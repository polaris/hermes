#ifndef _GameServer_H
#define _GameServer_H

#include "Game.h"
#include "Queue.h"
#include "Packet.h"
#include "ClientRegistry.h"
#include "Transceiver.h"

#include <unordered_map>

class Renderer;
class Clock;

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

    void renderWorld();
    void sendOutgoingPackets();

    unsigned int nextPlayerId_;
    unsigned int nextObjectId_;

    ClientRegistry clientRegistry_;

    Queue<Packet> packetPool_;
    Queue<Packet> incomingPackets_;
    Transceiver transceiver_;

    std::unordered_map<unsigned int, unsigned int> playerToObjectMap_;
};

#endif  // _GameServer_H
