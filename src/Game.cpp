#include "Game.h"
#include "GameObject.h"
#include "SpaceShip.h"
#include "Clock.h"

#include <SDL2/SDL.h>
#include <thread>

Game::Game(const char *title, unsigned int width, unsigned int height, unsigned int frameRate)
: window_(title, width, height)
, renderer_(window_)
, frameRate_(frameRate)
, inputHandler_(frameRate_)
, frameDuration_(1.0f / frameRate_) {
}

void Game::run(SpaceShip &spaceShip) {
    bool running = true;

    Clock clock;
    while (running) {
        SDL_Event event;
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

            renderer_.clear(0.25f, 0.25f, 0.25f);

            spaceShip.draw(renderer_);
            renderer_.present();

            const auto frameDuration = clock.getFrameDuration();
            if (frameDuration < frameDuration_) {
                const auto delay = static_cast<unsigned int>((frameDuration_ - frameDuration) * 1000.0f);
                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
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
