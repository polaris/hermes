#ifndef _Sprite_H
#define _Sprite_H

#include <SDL2/SDL.h>

#include <string>

class Game;

class Sprite {
public:
    Sprite(const std::string &filename, const Game &game);

    void draw(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const Game &game) const;

private:
    SDL_Texture *texture;
};

#endif  // _Sprite_H
