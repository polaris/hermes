#ifndef _GamePeer_H
#define _GamePeer_H

#include "Game.h"
#include "InputHandler.h"

class GamePeer : public Game {
public:
    GamePeer(unsigned int frameRate, Renderer& renderer, unsigned short port);

    GamePeer(unsigned int frameRate, Renderer& renderer, const char* masterAddress, unsigned short port);

private:
    void handleWillUpdateWorld(const Clock& clock) override;

    void handleDidUpdateWorld(const Clock& clock) override;

    void handleEvent(SDL_Event &event, bool& running) override;

    void finishFrame() override;
};

#endif	// _GamePeer_H
