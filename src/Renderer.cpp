#include "Renderer.h"
#include "Window.h"
#include "Vector2d.h"

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

void Renderer::clear(float r, float g, float b) {
    setDrawColor(r, g, b, 1);
    SDL_RenderClear(renderer_);
}

void Renderer::setDrawColor(float r, float g, float b, float a) {
    SDL_SetRenderDrawColor(renderer_, static_cast<Uint8>(r * 255), static_cast<Uint8>(g * 255),
        static_cast<Uint8>(b * 255), static_cast<Uint8>(a * 255));
}

void Renderer::drawLine(const Vector2d& a, const Vector2d& b) {
    SDL_RenderDrawLine(renderer_, static_cast<int>(a.x()), static_cast<int>(a.y()),
                                  static_cast<int>(b.x()), static_cast<int>(b.y()));
}

void Renderer::present() {
    SDL_RenderPresent(renderer_);
}
