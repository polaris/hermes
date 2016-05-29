#ifndef _GameServer_H
#define _GameServer_H

#include "Game.h"
#include "ServerWorld.h"
#include "BufferedQueue.h"
#include "ClientRegistry.h"
#include "Transceiver.h"
#include "LatencyEmulator.h"

#include <unordered_map>
#include <vector>

class Renderer;
class Clock;
class Packet;

class GameServer : public Game {
public:
    GameServer(unsigned int width, unsigned int height, unsigned int frameRate, unsigned int updateRate, unsigned int emulatedLatency, uint16_t port, Renderer& renderer);

private:
    void update(const Clock& clock) override;
    void handleEvent(SDL_Event &event, bool& running) override;

    void processIncomingPackets(const Clock& clock);
    void handlePacket(Packet* packet, const Clock& clock);
    void handleHello(Packet* packet, const Clock& clock);
    void handleInput(Packet* packet, const Clock& clock);
    void handleTick(Packet* packet, const Clock& clock);

    void checkForDisconnects(const Clock& clock);
    void renderWorld();
    void updateWorld();
    void sendOutgoingPackets(const Clock& clock);
    void sendStateUpdate(const Clock& clock);

    bool confirmCollision(uint32_t objectId1, uint32_t objectId2);
    void removedObject(uint32_t objectId);

    const unsigned int width_;
    const unsigned int height_;

    ServerWorld world_;

    const float updateInterval_;

    uint32_t nextPlayerId_;
    uint32_t nextObjectId_;
    std::unordered_map<uint32_t, uint32_t> playerToObjectMap_;
    std::unordered_map<uint32_t, uint32_t> objectToPlayerMap_;

    BufferedQueue bufferedQueue_;
    LatencyEmulator latencyEmulator_;
    Transceiver transceiver_;

    ClientRegistry clientRegistry_;

    float lastStateUpdate_;
};

#endif  // _GameServer_H
