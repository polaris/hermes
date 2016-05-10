#include "LocalSpaceShip.h"
#include "InputHandler.h"

LocalSpaceShip::LocalSpaceShip(const Renderer& renderer, InputHandler& inputHandler)
: SpaceShip(renderer)
, inputHandler_(inputHandler) {
}

void LocalSpaceShip::update(float elapsed) {
    auto move = inputHandler_.getAndClearPendingMove();
    if (move) {
        const auto& inputState = move->getInputState();
        rotate(inputState.desiredRightAmount * elapsed);
        rotate(-inputState.desiredLeftAmount * elapsed);
        thrust(inputState.desiredForwardAmount > 0);
    }

    SpaceShip::update(elapsed);
}

void LocalSpaceShip::read(Packet* packet) {
    SpaceShip::read(packet);

    // Replay all moves not yet processed by server.
}
