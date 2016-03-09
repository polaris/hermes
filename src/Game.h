#ifndef _Game_H
#define _Game_H

#include <SDL2/SDL.h>

class Game {
public:
    Game(const char *title, unsigned int width, unsigned int height);
    virtual ~Game();

    void run();

private:
    void setUp(const char *title, unsigned int width, unsigned int height);
    void tearDown();

    SDL_Window* window_;
    SDL_Renderer* renderer_;
    bool running_;
};

#endif  // _Game_H
