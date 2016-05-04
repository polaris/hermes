#ifndef _MoveList_H
#define _MoveList_H

#include "Move.h"

#include <deque>
#include <stdint.h>

class Packet;

class MoveList {
public:
    typedef std::deque<Move>::const_iterator const_iterator;

    MoveList();

    uint32_t getCount() const;

    const Move* addMove(const InputState& inputState, float timeStamp);

    void addMove(const Move& move);

    const Move* getLatestMove() const;

    void removeMovesUntil(float timeStamp);

    void clear();

    void write(Packet* packet) const;

    void read(Packet* packet);

    const_iterator begin() const {
        return moves_.begin();
    }
    
    const_iterator end() const {
        return moves_.end();
    }

private:
    InputState lastInputState_;
    float lastMoveTime_;
    std::deque<Move> moves_;
};

#endif  // _MoveList_H
