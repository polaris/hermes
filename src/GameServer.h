#ifndef _GameServer_H
#define _GameServer_H

#include "Game.h"
#include "Queue.h"
#include "Buffer.h"
#include "Transceiver.h"

class Renderer;
class Clock;

class GameServer : public Game {
public:
    GameServer(unsigned int frameRate, Renderer& renderer);

private:
    void handleWillUpdateWorld(const Clock& clock) override;
    void handleDidUpdateWorld(const Clock& clock) override;

    void processIncomingPackets();

    Queue<Buffer> bufferPool_;
    Queue<Buffer> incomingPackets_;
    Transceiver transceiver_;
};

#endif  // _GameServer_H
