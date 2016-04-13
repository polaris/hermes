#include "Game.h"
#include "GameObject.h"

#include <iostream>
#include <chrono>

#include <SDL2/SDL.h>

Game::Game(const char *title, unsigned int width, unsigned int height, unsigned int frameRate)
: window_(title, width, height)
, renderer_(window_)
, frameRate_(frameRate)
, frameDuration_(1000.0f / frameRate_)
, running_(false) {
}

void Game::run(GameObject &gameObject) {
    running_ = true;

    SDL_Event event;
    memset(&event, 0, sizeof(SDL_Event));

    auto lastTime = std::chrono::high_resolution_clock::now();
    while (running_) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) {
                running_ = false;
            } else {
                handleEvent(event);
            }
        } else {
            const auto currentTime = std::chrono::high_resolution_clock::now();
            const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count() / 1000.0f;
            lastTime = currentTime;

            gameObject.update(frameDuration_/1000.0f);

            renderer_.setDrawColor(0, 0, 1, 1);
            renderer_.clear();
            gameObject.draw(renderer_);
            renderer_.present();

            const auto frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - currentTime).count();
            if (frameTime < frameDuration_) {
               SDL_Delay(static_cast<unsigned int>(frameDuration_ - frameTime));
            }
        }
    }
}

void Game::handleEvent(SDL_Event &event) {
    switch(event.type) {
    case SDL_KEYDOWN:
        std::cout << event.key.keysym.sym << std::endl;
        // InputManager::sInstance->HandleInput( EIA_Pressed, inEvent->key.keysym.sym );
        break;
    case SDL_KEYUP:
        std::cout << event.key.keysym.sym << std::endl;
        //InputManager::sInstance->HandleInput( EIA_Released, inEvent->key.keysym.sym );
        break;
    default:
        break;
    }
}
