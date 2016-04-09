#ifndef _Sprite_H
#define _Sprite_H

#include <SDL2/SDL.h>

class Renderer;

class Sprite {
public:
    Sprite(const char *filename, const Renderer &renderer);
    ~Sprite();

    void draw(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const Renderer &renderer) const;

private:
    SDL_Texture *texture;
};

#endif  // _Sprite_H
