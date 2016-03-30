#include "Sprite.h"
#include "Game.h"

#include <iostream>

int main() {
    try {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            throw std::runtime_error(SDL_GetError());
        }

        Game game("Handle Latency", 640, 480);

        Sprite sprite("data/ninja.png", game.renderer());

        game.run(sprite);
    
        SDL_Quit();
    
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}
