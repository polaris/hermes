#include "Window.h"
#include "Renderer.h"
#include "GameClient.h"
#include "GameObjectRegistry.h"
#include "Logging.h"

#include <iostream>

int main() {
    try {
        INIT_LOGGING(LOG_LEVEL_DEBUG);

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            throw std::runtime_error(SDL_GetError());
        }
        atexit(SDL_Quit);

        Window window("Handle Latency - Client", 700, 5, 640, 480);
        
        Renderer renderer(window);
        
        registerGameObjects();

        GameClient gameClient(60, "127.0.0.1", 12345, renderer);

        gameClient.run();
        
        return 0;
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "spdlog exception: " << ex.what();
    } catch (const std::exception &ex) {
        ERROR("Exception: {0}", ex.what());
    }

    return 1;
}
