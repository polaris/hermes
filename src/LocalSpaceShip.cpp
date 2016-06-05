#include "LocalSpaceShip.h"
#include "InputHandler.h"
#include "Renderer.h"
#include "Utilities.h"
#include "Sound.h"

LocalSpaceShip::LocalSpaceShip(const Renderer& renderer, InputHandler& inputHandler, ShootFunc shootFunc, const Vector2d& position)
: SpaceShip(renderer, position)
, inputHandler_(inputHandler)
, shootFunc_(shootFunc)
, lastShot_(0)
, length(std::max(getWidth(), getHeight()))
, created_(true) {
}

LocalSpaceShip::LocalSpaceShip(const Renderer& renderer, InputHandler& inputHandler, ShootFunc shootFunc)
: LocalSpaceShip(renderer, inputHandler, shootFunc, Vector2d(0, 0)) {
}

LocalSpaceShip::LocalSpaceShip(const Renderer& renderer, InputHandler& inputHandler)
: LocalSpaceShip(renderer, inputHandler, nullptr) {
}

void LocalSpaceShip::update(float elapsed) {
    static float currentTime = 0.0f;
    static float lastTime = 0.0f;

    currentTime += elapsed;

    auto move = inputHandler_.getAndClearPendingMove();
    if (move) {
        const auto& inputState = move->getInputState();
        rotate(inputState.desiredRightAmount * elapsed);
        rotate(-inputState.desiredLeftAmount * elapsed);
        thrust(inputState.desiredForwardAmount > 0);
        if (inputState.shooting) {
            if (currentTime > lastTime + 0.5f) {
                Sound::getInstance()->playSound(0);
                lastTime = currentTime;
                if (shootFunc_) {
                    lastShot_ = shootFunc_(this, lastShot_);
                }
            }
        }
    }

    SpaceShip::update(elapsed);
}

void LocalSpaceShip::draw(Renderer& renderer) {
    SpaceShip::draw(renderer);
    renderer.drawRect(getPosition(), length, length);
}

void LocalSpaceShip::read(Packet* packet) {
    const auto oldPosition = position_;
    const auto oldVelocity = velocity_;
    const auto oldLookat = lookat_;
    const auto oldAcceleration = acceleration_;

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

    if (created_) {
        created_ = false;
    } else {
        position_ = lerp(position_, oldPosition, 0.5f);
        velocity_ = lerp(velocity_, oldVelocity, 0.5f);
        lookat_ = lerp(lookat_, oldLookat, 0.5f);
        acceleration_ = lerp(acceleration_, oldAcceleration, 0.5f);
    }
}
