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
