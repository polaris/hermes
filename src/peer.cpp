#include "Window.h"
#include "Renderer.h"
#include "GamePeer.h"
#include "GameObjectRegistry.h"
#include "Logging.h"

#include <boost/lexical_cast.hpp>

#include <iostream>

void printHelp();

int main(int argc, char** argv) {

    char* masterAddress = nullptr;
    unsigned short masterPort = 12345;

    int c = 0;
    while ((c = getopt(argc, argv, "m:p:h")) != -1) {
        switch (c) {
        case 'm':
            masterAddress = optarg;
            break;
        case 'p':
            masterPort = boost::lexical_cast<unsigned short>(optarg);
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

        Window window("Handle Latency - Peer", 640, 480);
        
        Renderer renderer(window);
        
        registerGameObjects();

        std::unique_ptr<GamePeer> gamePeer;
        if (masterAddress == nullptr) {
            INFO("I am the master listening for peers on port {0}.", masterPort);
            gamePeer = std::make_unique<GamePeer>(60, 4, renderer, masterPort);
        } else {
            INFO("I am a normal peer. Trying to connect to master at {0}:{1}.", masterAddress, masterPort);
            gamePeer = std::make_unique<GamePeer>(60, 4, renderer, masterAddress, masterPort);
        }

        gamePeer->run();
        
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
              << "  -m <address>  Pass the IP <address> of the master peer (omit this parameter to start peer as the master).\n"
              << "  -p <port>     Pass the UDP <port> of the master peer. Default port is 12345.\n"
              << "  -h            Display this information.\n"
              ;
}
