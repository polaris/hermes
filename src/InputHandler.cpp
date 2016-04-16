#include "InputHandler.h"

#include <SDL2/SDL.h>

#include <iostream>

InputHandler::InputHandler(float sampleRate)
: sampleInterval_(1.0f / sampleRate)
, nextTimeToSample_(0)
, hasInput(false) {
}

void InputHandler::handleInput(KeyAction keyAction, int keyCode) {
    if (SDLK_RIGHT == keyCode) {
        hasInput = true;
        if (keyAction == KeyAction::Down) {
            inputState_.desiredRightAmount = 1;
        } else {
            inputState_.desiredRightAmount = 0;
        }
    } else if (SDLK_LEFT == keyCode) {
        hasInput = true;
        if (keyAction == KeyAction::Down) {
            inputState_.desiredLeftAmount = 1;
        } else {
            inputState_.desiredLeftAmount = 0;
        }
    }
}

void InputHandler::update(float currentTime) {
    if (currentTime >= nextTimeToSample_) {
        if (hasInput == true) {
            pendingMove.reset(new Move(inputState_));
            hasInput = false;
        }
        nextTimeToSample_ += sampleInterval_;
    }
}
