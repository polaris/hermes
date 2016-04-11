#ifndef _Game_H
#define _Game_H

#include "Window.h"
#include "Renderer.h"

class Sprite;

class Game {
public:
    Game(const char *title, unsigned int width, unsigned int height, unsigned int frameRate);

    void run(const Sprite &sprite);

    const Renderer &getRenderer() const {
        return renderer_;
    }

private:
    Window window_;

    Renderer renderer_;

    const unsigned int frameRate_;

    const unsigned int frameMs_;

    bool running_;
};

#endif  // _Game_H
