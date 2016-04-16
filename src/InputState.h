#ifndef _InputState_H
#define _InputState_H

struct InputState {
    InputState()
    : desiredRightAmount(0)
    , desiredLeftAmount(0)
    , desiredForwardAmount(0)
    , shooting(false) {
    }

    float desiredRightAmount;
    float desiredLeftAmount;
    float desiredForwardAmount;
    bool shooting;
};

#endif  // _InputState_H
