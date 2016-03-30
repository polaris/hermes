#include "Renderer.h"
#include "Window.h"

#include <stdexcept>

Renderer::Renderer(const Window &window)
: renderer_(nullptr) {
    renderer_ = SDL_CreateRenderer(window.getSDLWindow(), -1, SDL_RENDERER_ACCELERATED);
    if (renderer_ == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }
}

Renderer::~Renderer() {
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
    }
    renderer_ = nullptr;
}

void Renderer::clear() {
    SDL_RenderClear(renderer_);
}

void Renderer::setDrawColor() {
    SDL_SetRenderDrawColor(renderer_, 128, 0, 0, 255);
}

void Renderer::present() {
    SDL_RenderPresent(renderer_);
}
