#include "Game.h"
#include "Clock.h"
#include "Renderer.h"

#include <thread>

Game::Game(unsigned int frameRate, Renderer& renderer)
: renderer_(renderer)
, frameDuration_(1.0f / frameRate) {
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
                processEvent(event, running);
            }
        } else {
            clock.update();

            handleWillUpdateWorld(clock);

            world_.update(frameDuration_);

            handleDidUpdateWorld(clock);

            finishFrame();

            const auto currentFrameDuration = clock.getFrameDuration();
            if (currentFrameDuration < frameDuration_) {
                const auto delay = static_cast<unsigned int>((frameDuration_ - currentFrameDuration) * 1000.0f);
                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            }
        }
    }
}

void Game::processEvent(SDL_Event &event, bool& running) {
    handleEvent(event, running);
}
