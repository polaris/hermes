#include "Game.h"
#include "GameObject.h"

#include <iostream>
#include <SDL2/SDL.h>

Game::Game(const char *title, unsigned int width, unsigned int height, unsigned int frameRate)
: window_(title, width, height)
, renderer_(window_)
, frameRate_(frameRate)
, frameMs_(1000/frameRate_)
, running_(false) {
}

void Game::run(GameObject &gameObject) {
    running_ = true;

    while (running_) {
        const auto frameStart = SDL_GetTicks();

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

        gameObject.update(frameMs_ / 1000.0f);

        renderer_.setDrawColor(0, 0, 1, 1);
        renderer_.clear();        
        gameObject.draw(renderer_);
        renderer_.present();

        const auto diff = SDL_GetTicks() - frameStart;
        const auto delay = diff > frameMs_ ? 0 : frameMs_ - diff;
        SDL_Delay(delay);
    }
}
