#include "Window.h"
#include "Renderer.h"
#include "GameServer.h"

#include <spdlog/spdlog.h>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>

void printHelp();

int main(int argc, char** argv) {
    unsigned short serverPort = 12345;

    int c = 0;
    while ((c = getopt(argc, argv, "p:h")) != -1) {
        switch (c) {
        case 'p':
            serverPort = boost::lexical_cast<unsigned short>(optarg);
            break;
        case 'h':
            printHelp();
            return 0;
        case '?':
            return 1;
        default:
            abort ();
        }
    }

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

void printHelp() {
    std::cout << "Usage: peer [options]\n"
              << "Options:\n"
              << "  -p <port>     Pass the UDP <port> of the server. This parameter is optional. Default is port 12345.\n"
              << "  -h            Display this information.\n"
              ;
}
