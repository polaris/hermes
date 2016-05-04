#include "InputState.h"
#include "Packet.h"

void InputState::write(Packet* packet) const {
    packet->write(desiredRightAmount);
    packet->write(desiredLeftAmount);
    packet->write(desiredForwardAmount);
    packet->write(shooting);
}

void InputState::read(Packet* packet) {
    packet->read(desiredRightAmount);
    packet->read(desiredLeftAmount);
    packet->read(desiredForwardAmount);
    packet->read(shooting);
}

bool operator ==(const InputState& lhs, const InputState& rhs) {
    return lhs.desiredRightAmount == rhs.desiredRightAmount &&
           lhs.desiredLeftAmount == rhs.desiredLeftAmount &&
           lhs.desiredForwardAmount == rhs.desiredForwardAmount &&
           lhs.shooting == rhs.shooting;
}

bool operator !=(const InputState& lhs, const InputState& rhs) {
    return !(lhs == rhs);
}
