#include "Game.h"

#include <iostream>

int main() {
    try {
        Game game("test", 640, 480);
        game.run();
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}
