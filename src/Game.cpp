#include "Game.h"
#include "Clock.h"
#include "Renderer.h"
#include "SpaceShip.h"

#include <thread>

Game::Game(unsigned int frameRate, Renderer& renderer)
: frameDuration_(1.0f / frameRate)
, renderer_(renderer)
, inputHandler_(frameRate) {
    world_.add(GameObjectPtr{new SpaceShip(renderer_, inputHandler_)});
}

void Game::run() {
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

            world_.update(frameDuration_);

            // process incoming packets

            renderer_.clear(0.25f, 0.25f, 0.25f);

            world_.draw(renderer_);
            renderer_.present();

            // send outgoing packets

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
