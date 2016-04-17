#include "Game.h"
#include "GameClient.h"
#include "SpaceShip.h"

#include <iostream>

int main() {
    try {
        GameClient gameClient("127.0.0.1", 12345, 32);

        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            throw std::runtime_error(SDL_GetError());
        }

        Game game("Handle Latency", 640, 480, 60);

        SpaceShip spaceShip(game.getRenderer());

        game.run(spaceShip);
    
        SDL_Quit();
    
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}
