#include "GamePeer.h"
#include "Renderer.h"

GamePeer::GamePeer(unsigned int frameRate, Renderer& renderer, unsigned short port)
: Game(frameRate, renderer) {
}

GamePeer::GamePeer(unsigned int frameRate, Renderer& renderer, const char* masterAddress, unsigned short port)
: Game(frameRate, renderer) {
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
            //inputHandler_.handleInput(KeyAction::Down, event.key.keysym.sym);
        }
        break;
    case SDL_KEYUP:
        //inputHandler_.handleInput(KeyAction::Up, event.key.keysym.sym);
        break;
    default:
        break;
    }
}

void GamePeer::finishFrame() {
}
