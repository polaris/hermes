#include "LocalSpaceShip.h"
#include "Renderer.h"
#include "InputHandler.h"
#include "Move.h"

LocalSpaceShip::LocalSpaceShip(const Renderer &renderer, InputHandler& inputHandler)
: SpaceShip(renderer)
, inputHandler_(inputHandler) {
}

void LocalSpaceShip::beforeUpdate(float elapsed) {
    auto move = inputHandler_.getAndClearPendingMove();
    if (move) {
        const auto& inputState = move->getInputState();
        rotate(inputState.desiredRightAmount * 5 * elapsed);
        rotate(-inputState.desiredLeftAmount * 5 * elapsed);
        thrust(inputState.desiredForwardAmount > 0);
    }
}
