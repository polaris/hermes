#include "Window.h"
#include "Renderer.h"
#include "GameClient.h"
#include "GameObjectRegistry.h"
#include "Logging.h"
#include "Sound.h"

#include <boost/lexical_cast.hpp>

#include <iostream>

void printHelp();

int main(int argc, char** argv) {
    char* serverAddress = nullptr;
    unsigned short serverPort = 12345;
    unsigned int emulatedLatency = 0;
    unsigned int stdDevLatencyMean = 0;

    int c = 0;
    while ((c = getopt(argc, argv, "s:p:l:d:h")) != -1) {
        switch (c) {
        case 's':
            serverAddress = optarg;
            break;
        case 'p':
            serverPort = boost::lexical_cast<unsigned short>(optarg);
            break;
        case 'l':
            emulatedLatency = boost::lexical_cast<unsigned int>(optarg);
            break;
        case 'd':
            stdDevLatencyMean = boost::lexical_cast<unsigned int>(optarg);
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

    if (serverAddress == nullptr) {
        std::cerr << "Error: no server address provided.\n";
        return -1;
    }

    try {
        INIT_LOGGING(LOG_LEVEL_DEBUG);

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
            throw std::runtime_error(SDL_GetError());
        }
        atexit(SDL_Quit);

        std::srand(static_cast<unsigned int>(std::time(0)));

        Sound::getInstance()->loadSound(0, "data/laser.wav");
        Sound::getInstance()->loadSound(1, "data/explosion1.wav");

        Window window("Handle Latency - Client", 700, 5, 640, 480);
        
        Renderer renderer(window);
        
        registerGameObjects();

        GameClient gameClient(60, emulatedLatency, stdDevLatencyMean, serverAddress, serverPort, renderer);

        gameClient.run();
        
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
              << "  -s <address>  Pass the IP <address> of the server. This parameter is required.\n"
              << "  -p <port>     Pass the UDP <port> of the server. This parameter is optional. Default is port 12345.\n"
              << "  -l <latency>  Pass the emulated latency in milliseconds. This parameter is optional. Default is 0 ms.\n"
              << "  -d <stddev>   Pass the standard deviation of the latency in milliseconds. This parameter is optional. Default is 0 ms.\n"
              << "  -h            Display this information.\n"
              ;
}
