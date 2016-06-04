#include "Animation.h"
#include "Renderer.h"

#include <SDL2/SDL_image.h>

#include <stdexcept>
#include <cmath>

Animation::Animation(const char* filename, unsigned int frameWidth, unsigned int frameHeight, const Renderer& renderer)
: frameWidth_(frameWidth)
, frameHeight_(frameHeight)
, frameCount_(0)
, currentFrame_(0)
, currentTime_(0)
, lastTime_(0)
, texture_(0) {
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
    frameCount_ = static_cast<unsigned int>(width) / frameWidth_;
}

Animation::~Animation() {
    tidyup();
}

void Animation::draw(int x, int y, const Renderer& renderer) const {
    if (currentFrame_ < frameCount_) {
        SDL_Rect src = { static_cast<int>(currentFrame_ * frameWidth_), 0, static_cast<int>(frameWidth_), static_cast<int>(frameHeight_) };
        SDL_Rect dst = { x, y, static_cast<int>(frameWidth_), static_cast<int>(frameHeight_) };
        SDL_RenderCopyEx(renderer.getSDLRenderer(), texture_, &src, &dst, 0, 0, SDL_FLIP_NONE);
    }
}

bool Animation::update(float elapsed) {
    currentTime_ += elapsed;
    if (currentTime_ > lastTime_ + 0.125f) {
        auto diff = currentTime_ - lastTime_;
        while (diff > 0.125f) {
            if (step()) {
                return true;
            }
            diff -= 0.125f;
        }
        lastTime_ = currentTime_ - diff;
    }
    return false;
}

unsigned int Animation::getWidth() const {
    return frameWidth_;
}

unsigned int Animation::getHeight() const {
    return frameHeight_;
}

void Animation::tidyup() {
    if (texture_ != nullptr) {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }
}

bool Animation::step() {
    if (currentFrame_ < frameCount_) {
        currentFrame_ += 1;
        return false;
    }
    return true;
}
