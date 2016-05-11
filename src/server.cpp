#include "Window.h"
#include "Renderer.h"
#include "GameServer.h"

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

        Window window("Handle Latency - Server", 5, 5, 640, 480);
        
        Renderer renderer(window);
        
        GameServer gameServer(60, 30, 12345, renderer);

        gameServer.run();
        
        return 0;
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "spdlog exception: " << ex.what();
    } catch (const std::exception &ex) {
        const auto errorMessage = boost::str(boost::format("Exception: %1%") % ex.what());
        spdlog::get("console")->error(errorMessage);
    }

    return 1;
}
