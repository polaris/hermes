#include "GamePeer.h"
#include "Renderer.h"

GamePeer::GamePeer(unsigned int frameRate, Renderer& renderer, unsigned short port)
: GamePeer(frameRate, renderer, nullptr, port) {
    currentState.reset(new GamePeer::Accepting{this});
}

GamePeer::GamePeer(unsigned int frameRate, Renderer& renderer, const char* masterAddress, unsigned short port)
: Game(frameRate, renderer)
, inputHandler_(30)
, latencyEstimator_(10)
, currentState(nullptr)
, nextState(nullptr)
, bufferedQueue_(1000)
, latencyEmulator_(bufferedQueue_, 150)
, transceiver_(latencyEmulator_) {
    currentState.reset(new GamePeer::Connecting{this});
}

void GamePeer::handleWillUpdateWorld(const Clock&) {
}

void GamePeer::handleDidUpdateWorld(const Clock&) {
    renderer_.clear(0.25f, 0.25f, 0.25f);
    //world_.draw(renderer_);
    renderer_.present();
}

void GamePeer::handleEvent(SDL_Event &event, bool& running) {
    switch(event.type) {
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            running = false;
        } else {
            inputHandler_.handleInput(KeyAction::Down, event.key.keysym.sym);
        }
        break;
    case SDL_KEYUP:
        inputHandler_.handleInput(KeyAction::Up, event.key.keysym.sym);
        break;
    default:
        break;
    }
}

void GamePeer::finishFrame() {
}

GamePeer::State::State(GamePeer* gamePeer)
: gamePeer_(gamePeer) {
}

GamePeer::Accepting::Accepting(GamePeer* gamePeer)
: State(gamePeer) {
}

void GamePeer::Accepting::handleWillUpdateWorld(const Clock&) {

}

void GamePeer::Accepting::handleIncomingPacket(Packet* packet, const Clock& clock) {

}

void GamePeer::Accepting::sendOutgoingPackets(const Clock& clock) {

}

GamePeer::Connecting::Connecting(GamePeer* gamePeer)
: State(gamePeer) {
}

void GamePeer::Connecting::handleWillUpdateWorld(const Clock&) {

}

void GamePeer::Connecting::handleIncomingPacket(Packet* packet, const Clock& clock) {

}

void GamePeer::Connecting::sendOutgoingPackets(const Clock& clock) {

}

GamePeer::Waiting::Waiting(GamePeer* gamePeer)
: State(gamePeer) {
}

void GamePeer::Waiting::handleWillUpdateWorld(const Clock&) {

}

void GamePeer::Waiting::handleIncomingPacket(Packet* packet, const Clock& clock) {

}

void GamePeer::Waiting::sendOutgoingPackets(const Clock& clock) {

}

GamePeer::Playing::Playing(GamePeer* gamePeer)
: State(gamePeer) {
}

void GamePeer::Playing::handleWillUpdateWorld(const Clock&) {

}

void GamePeer::Playing::handleIncomingPacket(Packet* packet, const Clock& clock) {

}

void GamePeer::Playing::sendOutgoingPackets(const Clock& clock) {

}
