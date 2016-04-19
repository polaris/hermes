#include "Window.h"
#include "Renderer.h"
#include "GameClient.h"
#include "InputHandler.h"
#include "SpaceShip.h"

#include <iostream>

int main() {
    try {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            throw std::runtime_error(SDL_GetError());
        }
        atexit(SDL_Quit);

        Window window("Handle Latency", 640, 480);
        
        Renderer renderer(window);
        
        GameClient gameClient(60, renderer);

        gameClient.run();
        
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}
