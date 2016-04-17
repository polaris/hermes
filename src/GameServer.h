#ifndef _GameServer_H
#define _GameServer_H

#include "Game.h"

class Renderer;
class Clock;

class GameServer : public Game {
public:
    GameServer(unsigned int frameRate, Renderer& renderer);

private:
    void handleWillUpdateWorld(const Clock& clock) override;
    void handleDidUpdateWorld(const Clock& clock) override;
};

#endif  // _GameServer_H
