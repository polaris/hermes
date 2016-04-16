#ifndef _Game_H
#define _Game_H

#include "Window.h"
#include "Renderer.h"
#include "InputHandler.h"

class GameObject;
class SpaceShip;

class Game {
public:
    Game(const char *title, unsigned int width, unsigned int height, unsigned int frameRate);

    void run(SpaceShip &spaceShip);

    const Renderer &getRenderer() const {
        return renderer_;
    }

private:
    void handleEvent(SDL_Event &event);

    Window window_;

    Renderer renderer_;

    InputHandler inputHandler_;

    const unsigned int frameRate_;

    const float frameDurationMs_;

    const float frameDurationSec_;
};

#endif  // _Game_H
