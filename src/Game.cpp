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

    bool leftPressed = false;

    auto lastTime = SDL_GetTicks();
    while (running_) {
        const auto currentTime = SDL_GetTicks();
        const auto elapsed = currentTime - lastTime;
        lastTime = currentTime;

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

        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_LEFT] == 1) {
            if (!leftPressed) {
                std::cout << "Left key down" << std::endl;
                leftPressed = true;
            }

            if (leftPressed) {
                std::cout << "Left pressed" << std::endl;
            }
        } else {
            if (leftPressed) {
                std::cout << "Left key up" << std::endl;
                leftPressed = false;
            }
        }


        gameObject.update(elapsed / 1000.0f);

        renderer_.setDrawColor(0, 0, 1, 1);
        renderer_.clear();
        gameObject.draw(renderer_);
        renderer_.present();

        const auto frameTime = SDL_GetTicks() - currentTime;
        if (frameTime < frameMs_) {
           SDL_Delay(frameMs_ - frameTime);
        }
    }
}
