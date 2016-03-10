#include "Sprite.h"
#include "Game.h"

#include <iostream>

int main() {
    try {
        Game game("Handle Latency", 640, 480);

        Sprite sprite("data/ninja.png", game);

        game.run(sprite);
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}
