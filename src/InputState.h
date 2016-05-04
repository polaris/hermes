#ifndef _InputState_H
#define _InputState_H

class Packet;

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

    void write(Packet* packet) const;

    void read(Packet* packet);

    friend bool operator ==(const InputState& lhs, const InputState& rhs);
    friend bool operator !=(const InputState& lhs, const InputState& rhs);
};

#endif  // _InputState_H
