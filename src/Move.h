#ifndef _Move_H
#define _Move_H

#include "InputState.h"

class Move {
public:
    explicit Move(const InputState &inputState);

    Move(const Move& rhs);

    Move& operator =(const Move& rhs);

    const InputState& getInputState() const;

private:
    InputState inputState_;
};

#endif  // _Move_H
