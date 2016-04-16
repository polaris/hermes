#include "Sprite.h"
#include "Renderer.h"

#include <SDL2/SDL_Image.h>

#include <stdexcept>
#include <cmath>

Sprite::Sprite(const char *filename, const Renderer &renderer)
: texture_(nullptr)
, width_(0)
, height_(0) {
    SDL_Surface *surface = IMG_Load(filename);
    if (surface == nullptr) {
        tidyup();
        throw std::runtime_error(IMG_GetError());
    }
    texture_ = SDL_CreateTextureFromSurface(renderer.getSDLRenderer(), surface);
    SDL_FreeSurface(surface);
    if (texture_ == nullptr) {
        tidyup();
        throw std::runtime_error(SDL_GetError());
    }
    Uint32 format = 0;
    int access = 0, width = 0, height = 0;
    if (SDL_QueryTexture(texture_, &format, &access, &width, &height) != 0) {
        tidyup();
        throw std::runtime_error(SDL_GetError());
    }
    width_ = static_cast<unsigned int>(width);
    height_ = static_cast<unsigned int>(height);
}

Sprite::~Sprite() {
    tidyup();
}

void Sprite::tidyup() {
    if (texture_ != nullptr) {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }
}

void Sprite::draw(int x, int y, const double &angle, const Renderer &renderer) const {
    draw(x, y, width_, height_, angle, renderer);
}

void Sprite::draw(int x, int y, unsigned int width, unsigned int height, const double &angle, const Renderer &renderer) const {
    SDL_Rect src = { 0, 0, static_cast<int>(width), static_cast<int>(height) };
    SDL_Rect dst = { x, y, static_cast<int>(width), static_cast<int>(height) };
    const auto angleDeg = angle * 180.0 / M_PI;
    SDL_RenderCopyEx(renderer.getSDLRenderer(), texture_, &src, &dst, angleDeg, 0, SDL_FLIP_NONE);
}
