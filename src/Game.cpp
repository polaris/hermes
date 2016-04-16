#include "Game.h"
#include "GameObject.h"
#include "SpaceShip.h"

#include <iostream>
#include <chrono>

#include <SDL2/SDL.h>

Game::Game(const char *title, unsigned int width, unsigned int height, unsigned int frameRate)
: window_(title, width, height)
, renderer_(window_)
, inputHandler_(30.0f)
, frameRate_(frameRate)
, frameDurationMs_(1000.0f / frameRate_)
, frameDurationSec_(1.0f / frameRate_) {
}

void Game::run(SpaceShip &spaceShip) {
    bool running = true;

    SDL_Event event;
    memset(&event, 0, sizeof(SDL_Event));

    const auto startTime = std::chrono::high_resolution_clock::now();
    auto lastTime = startTime;
    while (running) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) {
                running = false;
            } else {
                handleEvent(event);
            }
        } else {
            const auto currentTime = std::chrono::high_resolution_clock::now();
            const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count() / 1000.0f;
            lastTime = currentTime;

            inputHandler_.update(std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f);

            auto move = inputHandler_.getAndClearPendingMove();
            if (move) {
                spaceShip.rotate(-move->getInputState().desiredRightAmount * 10 * frameDurationSec_);
                spaceShip.rotate(move->getInputState().desiredLeftAmount * 10 * frameDurationSec_);
            }

            spaceShip.update(frameDurationSec_);

            renderer_.setDrawColor(0, 0, 1, 1);
            renderer_.clear();
            spaceShip.draw(renderer_);
            renderer_.present();

            const auto frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - currentTime).count();
            if (frameTime < frameDurationMs_) {
                const auto delay = static_cast<unsigned int>(frameDurationMs_ - frameTime);
                SDL_Delay(delay);
            }
        }
    }
}

void Game::handleEvent(SDL_Event &event) {
    switch(event.type) {
    case SDL_KEYDOWN:
        inputHandler_.handleInput(KeyAction::Down, event.key.keysym.sym);
        break;
    case SDL_KEYUP:
        inputHandler_.handleInput(KeyAction::Up, event.key.keysym.sym);
        break;
    default:
        break;
    }
}
