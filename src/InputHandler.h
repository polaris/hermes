#ifndef _InputHandler_H
#define _InputHandler_H

#include "InputState.h"
#include "Move.h"

#include <memory>

enum class KeyAction {
    Down, Up
};

class InputHandler {
public:
    explicit InputHandler(float sampleRate);

    void handleInput(KeyAction keyAction, int keyCode);

    void update(float currentTime);

    std::shared_ptr<Move> getAndClearPendingMove() {
        auto result = pendingMove;
        pendingMove.reset();
        return result;
    }

private:
    const float sampleInterval_;

    float nextTimeToSample_;

    InputState inputState_;

    std::shared_ptr<Move> pendingMove;
};

#endif  // _InputHandler_H
