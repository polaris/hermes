#include "Window.h"
#include "Renderer.h"
#include "GameClient.h"
#include "GameObjectRegistry.h"

#include <boost/log/trivial.hpp>

int main() {
    try {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            throw std::runtime_error(SDL_GetError());
        }
        atexit(SDL_Quit);

        Window window("Handle Latency - Client", 640, 480);
        
        Renderer renderer(window);
        
        registerGameObjects();

        GameClient gameClient(60, "127.0.0.1", 12345, renderer);

        gameClient.run();
        
        return 0;
    } catch (const std::exception &ex) {
       BOOST_LOG_TRIVIAL(fatal) << "Exception: " << ex.what();
       return 1;
    }
}
