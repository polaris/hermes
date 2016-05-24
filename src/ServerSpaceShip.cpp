#include "ServerSpaceShip.h"
#include "Renderer.h"
#include "ClientSession.h"
#include "MoveList.h"
#include "Move.h"

ServerSpaceShip::ServerSpaceShip(const Renderer& renderer, ClientSession* clientSession, ShootFunc shootFunc)
: SpaceShip(renderer)
, clientSession_(clientSession)
, shootFunc_(shootFunc)
, lastShot_(0) {
    setDirty();
}

void ServerSpaceShip::update(float elapsed) {
    SpaceShip::update(elapsed);

    MoveList& moveList = clientSession_->getMoveList();
    for (auto& move : moveList) {
        const auto& inputState = move.getInputState();
        float deltaTime = move.getDeltaTime();
        rotate(inputState.desiredRightAmount * deltaTime);
        rotate(-inputState.desiredLeftAmount * deltaTime);
        thrust(inputState.desiredForwardAmount > 0);
        if (inputState.shooting) {
            lastShot_ = shootFunc_(this, lastShot_);
        }
    }
    moveList.clear();
}
