#include "Window.h"
#include "Renderer.h"
#include "GameServer.h"
#include "Logging.h"

#include <boost/lexical_cast.hpp>

#include <iostream>

void printHelp();

int main(int argc, char** argv) {
    unsigned short serverPort = 12345;
    unsigned int emulatedLatency = 0;

    int c = 0;
    while ((c = getopt(argc, argv, "p:l:h")) != -1) {
        switch (c) {
        case 'p':
            serverPort = boost::lexical_cast<unsigned short>(optarg);
            break;
        case 'l':
            emulatedLatency = boost::lexical_cast<unsigned int>(optarg);
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
        INIT_LOGGING(LOG_LEVEL_DEBUG);

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            throw std::runtime_error(SDL_GetError());
        }
        atexit(SDL_Quit);

        Window window("Handle Latency - Server", 5, 5, 640, 480);
        
        Renderer renderer(window);
        
        GameServer gameServer(60, 30, emulatedLatency, serverPort, renderer);

        gameServer.run();
        
        return 0;
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "spdlog exception: " << ex.what();
    } catch (const std::exception &ex) {
        ERROR("Exception: {0}", ex.what());
    }

    return 1;
}

void printHelp() {
    std::cout << "Usage: peer [options]\n"
              << "Options:\n"
              << "  -p <port>     Pass the UDP <port> of the server. This parameter is optional. Default is port 12345.\n"
              << "  -l <latency>  Pass the emulated latency in milliseconds. This parameter is optional. Default is 0 ms.\n"
              << "  -h            Display this information.\n"
              ;
}
