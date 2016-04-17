#ifndef _Game_H
#define _Game_H

#include "World.h"
#include "InputHandler.h"

#include <SDL2/SDL.h>

class GameObject;
class Renderer;

class Game {
public:
    Game(unsigned int frameRate, Renderer& renderer);

    void run();

private:
    void handleEvent(SDL_Event &event, bool& running);

    const float frameDuration_;

    Renderer& renderer_;

    World world_;

    InputHandler inputHandler_;
};

#endif  // _Game_H
