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
    }
}

void InputHandler::update(float currentTime) {
    if (currentTime >= nextTimeToSample_) {
        pendingMove_.reset(new Move(moveList_.addMove(inputState_, currentTime)));
        nextTimeToSample_ += sampleInterval_;
    }
}

float InputHandler::getSampleInterval() const {
    return sampleInterval_;
}

std::shared_ptr<Move> InputHandler::getAndClearPendingMove() {
    auto result = pendingMove_;
    pendingMove_.reset();
    return result;
}

MoveList& InputHandler::getMoveList() {
    return moveList_;
}
