#include "Game.h"
#include "Sprite.h"

#include <stdexcept>

Game::Game(const char *title, unsigned int width, unsigned int height)
: window_(nullptr)
, renderer_(nullptr)
, running_(false) {
    setUp(title, width, height);
}

Game::~Game() {
    tearDown();
}

void Game::setUp(const char *title, unsigned int width, unsigned int height) {
    if (SDL_Init(SDL_INIT_EVERYTHING) >= 0) {
        window_ = SDL_CreateWindow(
            title,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            static_cast<int>(width),
            static_cast<int>(height),
            SDL_WINDOW_SHOWN);
        if (window_ != nullptr) {
            renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
            if (renderer_ == nullptr) {
                tearDown();
                throw std::runtime_error(SDL_GetError());
            }
        } else {
            tearDown();
            throw std::runtime_error(SDL_GetError());
        }
    } else {
        throw std::runtime_error(SDL_GetError());
    }
}

void Game::tearDown() {
    if (window_) {
        SDL_DestroyWindow(window_);
    }
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
    }
    SDL_Quit();
}

void Game::run(const Sprite &sprite) {
    running_ = true;
    while (running_) {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                running_ = false;
                break;
            default:
            break;
            }
        }
        
        // update

        // Render
        SDL_SetRenderDrawColor(renderer_, 128, 0, 0, 255);
        SDL_RenderClear(renderer_);
        
        sprite.draw(0, 0, 56, 80, *this);
        
        SDL_RenderPresent(renderer_);
    }
}

