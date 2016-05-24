#include "InputHandler.h"
#include "Move.h"

#include <SDL2/SDL.h>

InputHandler::InputHandler(unsigned int sampleRate)
: sampleInterval_(1.0f / static_cast<float>(sampleRate))
, nextTimeToSample_(0)
, inputState_()
, pendingMove_(nullptr)
, moveList_() {
}

void InputHandler::handleInput(KeyAction keyAction, int keyCode) {
    if (SDLK_RIGHT == keyCode) {
        if (keyAction == KeyAction::Down) {
            inputState_.desiredRightAmount = 5;
        } else {
            inputState_.desiredRightAmount = 0;
        }
    } else if (SDLK_LEFT == keyCode) {
        if (keyAction == KeyAction::Down) {
            inputState_.desiredLeftAmount = 5;
        } else {
            inputState_.desiredLeftAmount = 0;
        }
    } else if (SDLK_UP == keyCode) {
        if (keyAction == KeyAction::Down) {
            inputState_.desiredForwardAmount = 1;
        } else {
            inputState_.desiredForwardAmount = 0;
        }
    } else if (SDLK_SPACE == keyCode) {
        if (keyAction == KeyAction::Down) {
            inputState_.shooting = true;
        } else {
            inputState_.shooting = false;
        }
    }
}

void InputHandler::update(float currentTime) {
    if (currentTime >= nextTimeToSample_) {
        pendingMove_ = moveList_.addMove(inputState_, currentTime);
        nextTimeToSample_ += sampleInterval_;
    }
}

float InputHandler::getSampleInterval() const {
    return sampleInterval_;
}

const Move* InputHandler::getAndClearPendingMove() {
    auto result = pendingMove_;
    pendingMove_ = nullptr;
    return result;
}

MoveList& InputHandler::getMoveList() {
    return moveList_;
}
