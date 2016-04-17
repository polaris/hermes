#include "GameServer.h"
#include "Renderer.h"
#include "Clock.h"

GameServer::GameServer(unsigned int frameRate, Renderer& renderer)
: Game(frameRate, renderer) {
}

void GameServer::handleWillUpdateWorld(const Clock& clock) {
    inputHandler_.update(clock.getGameTime());
}

void GameServer::handleDidUpdateWorld(const Clock&) {
    renderer_.clear(0.25f, 0.25f, 0.25f);
    world_.draw(renderer_);
    renderer_.present();
}
