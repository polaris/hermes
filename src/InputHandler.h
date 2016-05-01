#ifndef _InputHandler_H
#define _InputHandler_H

#include "InputState.h"
#include "MoveList.h"

#include <memory>

enum class KeyAction {
    Down, Up
};

class Move;

class InputHandler {
public:
    explicit InputHandler(float sampleRate);

    void handleInput(KeyAction keyAction, int keyCode);

    void update(float currentTime);

    float getSampleInterval() const;

    std::shared_ptr<Move> getAndClearPendingMove();

    MoveList& getMoveList();

private:
    const float sampleInterval_;

    float nextTimeToSample_;

    InputState inputState_;

    std::shared_ptr<Move> pendingMove_;

    MoveList moveList_;
};

#endif  // _InputHandler_H
