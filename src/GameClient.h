#ifndef _GameClient_H
#define _GameClient_H

#include "Game.h"

class Renderer;
class Clock;

class GameClient : public Game {
public:
    GameClient(unsigned int frameRate, Renderer& renderer);

private:
    void handleWillUpdateWorld(const Clock& clock) override;
    void handleDidUpdateWorld(const Clock& clock) override;
};

#endif  // _GameClient_H
