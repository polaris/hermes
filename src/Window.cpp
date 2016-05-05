#include "Window.h"

#include <stdexcept>

Window::Window(const char *title, unsigned int xpos, unsigned int ypos, unsigned int width, unsigned int height)
: window_(nullptr) {
    window_ = SDL_CreateWindow(title, static_cast<int>(xpos), static_cast<int>(ypos),
        static_cast<int>(width), static_cast<int>(height), SDL_WINDOW_SHOWN);
    if (window_ == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }
}

Window::Window(const char *title, unsigned int width, unsigned int height)
: window_(nullptr) {
    window_ = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        static_cast<int>(width), static_cast<int>(height), SDL_WINDOW_SHOWN);
    if (window_ == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }
}

Window::~Window() {
    if (window_) {
        SDL_DestroyWindow(window_);
    }
    window_ = nullptr;
}
