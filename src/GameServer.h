#ifndef _GameServer_H
#define _GameServer_H

#include "Game.h"
#include "BufferedQueue.h"
#include "ClientRegistry.h"
#include "Transceiver.h"

#include <unordered_map>

class Renderer;
class Clock;
class Packet;

class GameServer : public Game {
public:
    GameServer(unsigned int frameRate, uint16_t port, Renderer& renderer);

private:
    void handleWillUpdateWorld(const Clock& clock) override;
    void handleDidUpdateWorld(const Clock& clock) override;
    void handleEvent(SDL_Event &event, bool& running) override;

    void processIncomingPackets(const Clock& clock);
    void handlePacket(Packet* packet, const Clock& clock);
    void handleHello(Packet* packet, const Clock& clock);
    void handleInput(Packet* packet, const Clock& clock);
    void handleTick(Packet* packet, const Clock& clock);

    void renderWorld();
    void sendOutgoingPackets();

    uint32_t nextPlayerId_;
    uint32_t nextObjectId_;
    std::unordered_map<uint32_t, uint32_t> playerToObjectMap_;

    BufferedQueue bufferedQueue_;
    Transceiver transceiver_;

    ClientRegistry clientRegistry_;
};

#endif  // _GameServer_H
