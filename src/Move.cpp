#include "Move.h"

Move::Move(const InputState& inputState, float timeStamp, float deltaTime)
: inputState_(inputState)
, timeStamp_(timeStamp)
, deltaTime_(deltaTime) {
}

Move::Move(const Move& rhs)
: inputState_(rhs.inputState_)
, timeStamp_(rhs.timeStamp_)
, deltaTime_(rhs.deltaTime_) {
}

Move& Move::operator =(const Move& rhs) {
    inputState_ = rhs.inputState_;
    timeStamp_ = rhs.timeStamp_;
    deltaTime_ = rhs.deltaTime_;
    return *this;
}

const InputState& Move::getInputState() const {
    return inputState_;
}

float Move::getTimeStamp() const {
    return timeStamp_;
}

float Move::getDeltaTime() const {
    return deltaTime_;
}
