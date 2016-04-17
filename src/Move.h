#ifndef _Move_H
#define _Move_H

#include "InputState.h"

class Move {
public:
    explicit Move(const InputState &inputState, float timeStamp, float deltaTime);

    Move(const Move& rhs);

    Move& operator =(const Move& rhs);

    const InputState& getInputState() const;

    float getTimeStamp() const;

    float getDeltaTime() const;

private:
    InputState inputState_;
    float timeStamp_;
    float deltaTime_;
};

#endif  // _Move_H
