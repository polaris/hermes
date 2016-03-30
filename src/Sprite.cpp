#include "Sprite.h"
#include "Renderer.h"

#include <SDL2/SDL_Image.h>

Sprite::Sprite(const std::string &filename, const Renderer &renderer)
: texture(nullptr) {

    SDL_Surface *surface = IMG_Load(filename.c_str());    
    if (surface == nullptr) {
        throw std::runtime_error(IMG_GetError());
    }
    
    texture = SDL_CreateTextureFromSurface(renderer.getSDLRenderer(), surface);

    SDL_FreeSurface(surface);

    if (texture == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }
}

void Sprite::draw(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const Renderer &renderer) const {
    SDL_Rect src;
    SDL_Rect dst;
    
    src.x = 0;
    src.y = 0;
    src.w = dst.w = static_cast<int>(width);
    src.h = dst.h = static_cast<int>(height);
    dst.x = static_cast<int>(x);
    dst.y = static_cast<int>(y);
    
    SDL_RenderCopyEx(renderer.getSDLRenderer(), texture, &src, &dst, 0, 0, SDL_FLIP_NONE);
}
