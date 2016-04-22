#include "GameClient.h"
#include "Renderer.h"
#include "Clock.h"

#include <iostream>

GameClient::GameClient(unsigned int frameRate, Renderer& renderer)
: Game(frameRate, renderer)
, currentState(new GameClient::Connecting{this})
, packetPool_(100 , [] () { return new Packet(1500); })
, incomingPackets_(100)
, transceiver_(packetPool_, incomingPackets_) {
}

void GameClient::handleWillUpdateWorld(const Clock& clock) {
    currentState->handleWillUpdateWorld(clock);
}

void GameClient::handleDidUpdateWorld(const Clock& clock) {
    currentState->handleDidUpdateWorld(clock);
}

void GameClient::setState(std::shared_ptr<State>& newState) {
    currentState = newState;
}

GameClient::State::State(GameClient* gameClient)
: gameClient_(gameClient) {
}

GameClient::Connecting::Connecting(GameClient* gameClient)
: State(gameClient)
, lastHelloTime_(0) {
}

void GameClient::Connecting::handleWillUpdateWorld(const Clock& clock) {
    gameClient_->inputHandler_.update(clock.getGameTime());
}

void GameClient::Connecting::handleDidUpdateWorld(const Clock& clock) {
    processIncomingPackets();

    gameClient_->renderer_.clear(0.25f, 0.25f, 0.25f);
    gameClient_->world_.draw(gameClient_->renderer_);
    gameClient_->renderer_.present();

    sendHello(clock);
}

void GameClient::Connecting::processIncomingPackets() {
    auto packet = gameClient_->incomingPackets_.pop();
    if (packet) {
        std::cout << "Received a packet\n";
        packet->reset();
        gameClient_->packetPool_.push(packet);
    }
}

void GameClient::Connecting::sendHello(const Clock& clock) {
    auto now = clock.getTime();

    if (now > lastHelloTime_ + 1) {
        auto packet = gameClient_->packetPool_.pop();
        packet->setEndpoint("127.0.0.1", 12345);
        packet->write("HELO\0", 6);
        gameClient_->transceiver_.sendTo(packet);
        lastHelloTime_ = now;
    }
}

GameClient::Connected::Connected(GameClient* gameClient)
: State(gameClient) {
}

void GameClient::Connected::handleWillUpdateWorld(const Clock& clock) {
    gameClient_->inputHandler_.update(clock.getGameTime());
}

void GameClient::Connected::handleDidUpdateWorld(const Clock&) {
    gameClient_->renderer_.clear(0.25f, 0.25f, 0.25f);
    gameClient_->world_.draw(gameClient_->renderer_);
    gameClient_->renderer_.present();
}
