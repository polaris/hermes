#include "GameServer.h"
#include "Renderer.h"
#include "Clock.h"

#include <iostream>

GameServer::GameServer(unsigned int frameRate, Renderer& renderer)
: Game(frameRate, renderer)
, bufferPool_(100)
, incomingPackets_(100)
, transceiver_(12345, bufferPool_, incomingPackets_) {
    for (std::size_t i = 0; i < 100; i++) {
        bufferPool_.push(new Buffer(1500));
    }
    transceiver_.startReception();
}

void GameServer::handleWillUpdateWorld(const Clock&) {
    processIncomingPackets();

    // check for disconnects

    // respawn cats
}

void GameServer::processIncomingPackets() {
    auto buffer = incomingPackets_.pop();
    if (buffer) {
        std::cout << "Received a packet\n";
        bufferPool_.push(buffer);
    }
}

void GameServer::handleDidUpdateWorld(const Clock&) {
    renderer_.clear(0.25f, 0.25f, 0.25f);
    world_.draw(renderer_);
    renderer_.present();

    // send outgoing packets
}
