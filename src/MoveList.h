#ifndef _MoveList_H
#define _MoveList_H

#include "Move.h"

#include <deque>

class Packet;

class MoveList {
public:
    MoveList();

    std::size_t getCount() const;

    const Move& addMove(const InputState& inputState, float timeStamp);

    void addMove(const Move& move);

    const Move& getLatestMove() const;

    void removeMovesUntil(float timeStamp);

    void clear();

    void write(Packet* packet) const;

    void read(Packet* packet);

private:
    float lastMoveTime_;
    std::deque<Move> moves_;
};

#endif  // _MoveList_H
