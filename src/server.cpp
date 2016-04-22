#include "Window.h"
#include "Renderer.h"
#include "GameServer.h"
#include "InputHandler.h"
#include "SpaceShip.h"

#include <boost/log/trivial.hpp>

int main() {
    try {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            throw std::runtime_error(SDL_GetError());
        }
        atexit(SDL_Quit);

        Window window("Handle Latency", 640, 480);
        
        Renderer renderer(window);
        
        GameServer gameServer(60, renderer);

        gameServer.run();
        
        return 0;
    } catch (const std::exception &ex) {
        BOOST_LOG_TRIVIAL(fatal) << "Exception: " << ex.what();
        return 1;
    }
}
