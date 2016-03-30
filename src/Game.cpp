#include "Game.h"
#include "Sprite.h"

#include <SDL2/SDL.h>

Game::Game(const char *title, unsigned int width, unsigned int height)
: window_(title, width, height)
, renderer_(window_)
, running_(false) {
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

        renderer_.setDrawColor(0, 0, 1, 1);
        renderer_.clear();
        
        sprite.draw(0, 0, 56, 80, renderer_);

        renderer_.present();        
    }
}
