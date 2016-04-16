#include "Move.h"

Move::Move(const InputState& inputState)
: inputState_(inputState) {
}

Move::Move(const Move& rhs)
: inputState_(rhs.inputState_) {
}

Move& Move::operator =(const Move& rhs) {
    inputState_ = rhs.inputState_;
    return *this;
}

const InputState& Move::getInputState() const {
    return inputState_;
}
