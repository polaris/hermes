#include "Window.h"
#include "Renderer.h"
#include "Game.h"
#include "Transceiver.h"
#include "InputHandler.h"
#include "SpaceShip.h"

#include <iostream>

int main() {
    try {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            throw std::runtime_error(SDL_GetError());
        }

        Window window("Handle Latency", 640, 480);
        
        Renderer renderer(window);
        
        Game game(60, renderer);

        game.run();
    
        SDL_Quit();
    
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}
