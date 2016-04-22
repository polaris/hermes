#include "GameServer.h"
#include "Renderer.h"
#include "Clock.h"

#include <iostream>

GameServer::GameServer(unsigned int frameRate, Renderer& renderer)
: Game(frameRate, renderer)
, packetPool_(100, [] () { return new Packet(1500); })
, incomingPackets_(100)
, transceiver_(12345, packetPool_, incomingPackets_) {
}

void GameServer::handleWillUpdateWorld(const Clock&) {
    processIncomingPackets();

    // check for disconnects

    // respawn cats
}

void GameServer::processIncomingPackets() {
    auto packet = incomingPackets_.pop();
    if (packet) {
        std::cout << "Received a packet\n";

        auto sendBuffer = packetPool_.pop();
        sendBuffer->setEndpoint(packet->getEndpoint());
        sendBuffer->write("HELO\0", 6);
        transceiver_.sendTo(sendBuffer);

        packet->reset();
        packetPool_.push(packet);
    }
}

void GameServer::handleDidUpdateWorld(const Clock&) {
    renderer_.clear(0.25f, 0.25f, 0.25f);
    world_.draw(renderer_);
    renderer_.present();

    // send outgoing packets
}
