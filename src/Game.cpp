#include "Game.h"
#include "GameObject.h"
#include "SpaceShip.h"
#include "Clock.h"

#include <SDL2/SDL.h>

Game::Game(const char *title, unsigned int width, unsigned int height, unsigned int frameRate)
: window_(title, width, height)
, renderer_(window_)
, frameRate_(frameRate)
, inputHandler_(frameRate_)
, frameDuration_(1.0f / frameRate_) {
}

void Game::run(SpaceShip &spaceShip) {
    bool running = true;

    SDL_Event event;
    memset(&event, 0, sizeof(SDL_Event));

    Clock clock;
    while (running) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) {
                running = false;
            } else {
                handleEvent(event, running);
            }
        } else {
            clock.update();

            inputHandler_.update(clock.getGameTime());

            auto move = inputHandler_.getAndClearPendingMove();
            if (move) {
                const auto& inputState = move->getInputState();
                spaceShip.rotate(inputState.desiredRightAmount * 5 * frameDuration_);
                spaceShip.rotate(-inputState.desiredLeftAmount * 5 * frameDuration_);
                spaceShip.thrust(inputState.desiredForwardAmount > 0);
            }

            spaceShip.update(frameDuration_);

            renderer_.setDrawColor(0, 0, 1, 1);
            renderer_.clear();
            renderer_.setDrawColor(1, 0, 0, 1);
            spaceShip.draw(renderer_);
            renderer_.present();

            const auto frameDuration = clock.getFrameDuration();
            if (frameDuration < frameDuration_) {
                const auto delay = static_cast<unsigned int>((frameDuration_ - frameDuration) * 1000.0f);
                SDL_Delay(delay);
            }
        }
    }
}

void Game::handleEvent(SDL_Event &event, bool& running) {
    switch(event.type) {
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            running = false;
        } else {
            inputHandler_.handleInput(KeyAction::Down, event.key.keysym.sym);
        }
        break;
    case SDL_KEYUP:
        inputHandler_.handleInput(KeyAction::Up, event.key.keysym.sym);
        break;
    default:
        break;
    }
}
