#include "MoveList.h"
#include "Packet.h"

MoveList::MoveList()
: lastMoveTime_(0) {
}

std::size_t MoveList::getCount() const {
    return moves_.size();
}

const Move& MoveList::addMove(const InputState& inputState, float timeStamp) {
    const auto deltaTime = timeStamp - lastMoveTime_;
    moves_.emplace_back(inputState, timeStamp, deltaTime);
    lastMoveTime_ = timeStamp;
    return moves_.back();
}

void MoveList::addMove(const Move& move) {
    const auto timeStamp = move.getTimeStamp();
    if (timeStamp > lastMoveTime_) {
        const auto deltaTime = timeStamp - lastMoveTime_;
        moves_.emplace_back(move.getInputState(), timeStamp, deltaTime);
        lastMoveTime_ = timeStamp;
    }
}

const Move& MoveList::getLatestMove() const {
    if (moves_.empty()) {
        throw std::logic_error("list is empty");
    }
    return moves_.back();
}

void MoveList::removeMovesUntil(float timeStamp) {
    moves_.erase(std::remove_if(moves_.begin(), moves_.end(), [timeStamp] (const Move& move) {
        return move.getTimeStamp() <= timeStamp;
    }), moves_.end());
}

void MoveList::clear() {
    moves_.clear();
}

void MoveList::write(Packet* packet) const {
    // const auto moveCount = getCount();
    // auto firstMoveIndex = moveCount - 3;
    // if (firstMoveIndex < 3) {
    //     firstMoveIndex = 0;
    // }
    packet->write(getCount());
    for (const auto& move : moves_) {
        move.write(packet);
    }
}

void MoveList::read(Packet* packet) {
    std::size_t count = 0;
    packet->read(count);
    for (std::size_t i = 0; i < count; i++) {
        Move move;
        move.read(packet);
        addMove(move);
    }
}
