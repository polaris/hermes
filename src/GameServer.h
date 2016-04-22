#ifndef _GameServer_H
#define _GameServer_H

#include "Game.h"
#include "Queue.h"
#include "Packet.h"
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
    void handlePacket(Packet* packet);
    void handleHello(Packet* packet);

    Queue<Packet> packetPool_;
    Queue<Packet> incomingPackets_;
    Transceiver transceiver_;
};

#endif  // _GameServer_H
