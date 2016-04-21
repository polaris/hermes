#include "GameServer.h"
#include "Renderer.h"
#include "Clock.h"

#include <iostream>

GameServer::GameServer(unsigned int frameRate, Renderer& renderer)
: Game(frameRate, renderer)
, bufferPool_(100, [] () { return new Buffer(1500); })
, incomingPackets_(100)
, transceiver_(12345, bufferPool_, incomingPackets_) {
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

        auto sendBuffer = bufferPool_.pop();
        sendBuffer->setEndpoint(buffer->getEndpoint());
        sendBuffer->write("HELO\0", 6);
        transceiver_.sendTo(sendBuffer);

        buffer->reset();
        bufferPool_.push(buffer);
    }
}

void GameServer::handleDidUpdateWorld(const Clock&) {
    renderer_.clear(0.25f, 0.25f, 0.25f);
    world_.draw(renderer_);
    renderer_.present();

    // send outgoing packets
}
