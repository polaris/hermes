#include "InputHandler.h"

#include <SDL2/SDL.h>

#include <iostream>

InputHandler::InputHandler(float sampleRate)
: sampleInterval_(1.0f / sampleRate)
, nextTimeToSample_(0) {
}

void InputHandler::handleInput(KeyAction keyAction, int keyCode) {
    if (SDLK_RIGHT == keyCode) {
        if (keyAction == KeyAction::Down) {
            inputState_.desiredRightAmount = 1;
        } else {
            inputState_.desiredRightAmount = 0;
        }
    } else if (SDLK_LEFT == keyCode) {
        if (keyAction == KeyAction::Down) {
            inputState_.desiredLeftAmount = 1;
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
        pendingMove.reset(new Move(inputState_));
        nextTimeToSample_ += sampleInterval_;
    }
}
