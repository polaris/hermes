#include "Window.h"
#include "Renderer.h"
#include "GameClient.h"
#include "GameObjectRegistry.h"

#include <spdlog/spdlog.h>
#include <boost/format.hpp>

#include <iostream>

int main() {
    try {
        auto console = spdlog::stdout_logger_mt("console", true);
        console->set_level(spdlog::level::debug);

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
        const auto errorMessage = boost::str(boost::format("Exception: %1%") % ex.what());
        spdlog::get("console")->error(errorMessage);
    }

    return 1;
}
