#ifndef _Game_H
#define _Game_H

#include "Window.h"
#include "Renderer.h"

class GameObject;

class Game {
public:
    Game(const char *title, unsigned int width, unsigned int height, unsigned int frameRate);

    void run(GameObject &gameObject);

    const Renderer &getRenderer() const {
        return renderer_;
    }

private:
    void handleEvent(SDL_Event &event);

    Window window_;

    Renderer renderer_;

    const unsigned int frameRate_;

    const float frameDuration_;

    bool running_;
};

#endif  // _Game_H
