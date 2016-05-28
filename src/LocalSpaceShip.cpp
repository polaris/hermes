#include "LocalSpaceShip.h"
#include "InputHandler.h"

LocalSpaceShip::LocalSpaceShip(const Renderer& renderer, InputHandler& inputHandler, ShootFunc shootFunc)
: SpaceShip(renderer)
, inputHandler_(inputHandler)
, shootFunc_(shootFunc)
, lastShot_(0) {
}

LocalSpaceShip::LocalSpaceShip(const Renderer& renderer, InputHandler& inputHandler)
: LocalSpaceShip(renderer, inputHandler, nullptr) {
}

void LocalSpaceShip::update(float elapsed) {
    auto move = inputHandler_.getAndClearPendingMove();
    if (move) {
        const auto& inputState = move->getInputState();
        rotate(inputState.desiredRightAmount * elapsed);
        rotate(-inputState.desiredLeftAmount * elapsed);
        thrust(inputState.desiredForwardAmount > 0);
        if (inputState.shooting && shootFunc_) {
            lastShot_ = shootFunc_(this, lastShot_);
        }
    }

    SpaceShip::update(elapsed);
}

void LocalSpaceShip::read(Packet* packet) {
    SpaceShip::read(packet);

    MoveList& moveList = inputHandler_.getMoveList();
    for (auto& move : moveList) {
        const auto& inputState = move.getInputState();
        float deltaTime = move.getDeltaTime();
        rotate(inputState.desiredRightAmount * deltaTime);
        rotate(-inputState.desiredLeftAmount * deltaTime);
        thrust(inputState.desiredForwardAmount > 0);
        SpaceShip::update(deltaTime);
    }
}
