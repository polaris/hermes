#ifndef _Game_H
#define _Game_H

#include "World.h"
#include "InputHandler.h"

#include <SDL2/SDL.h>

class GameObject;
class Renderer;
class Clock;

class Game {
public:
    Game(unsigned int frameRate, Renderer& renderer);

    void run();

protected:    
    virtual void handleWillUpdateWorld(const Clock& clock) = 0;

    virtual void handleDidUpdateWorld(const Clock& clock) = 0;

    Renderer& renderer_;

    World world_;

    InputHandler inputHandler_;

private:
    void handleEvent(SDL_Event &event, bool& running);

    const float frameDuration_;
};

#endif  // _Game_H
