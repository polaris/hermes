#include "MoveList.h"
#include "Packet.h"
#include "catch.hpp"

TEST_CASE("MoveList::addMove(...) returns the latest move") {
    MoveList moveList;

    const auto latestMove = moveList.addMove(InputState{}, 1.0f);
    REQUIRE(latestMove->getTimeStamp() == 1.0f);
    REQUIRE(latestMove->getDeltaTime() == 1.0f);
}

TEST_CASE("MoveList::addMove(...) calculates the correct delta time") {
    MoveList moveList;

    moveList.addMove(InputState{}, 1.0f);
    REQUIRE(moveList.getLatestMove()->getDeltaTime() == 1.0f);

    moveList.addMove(InputState{}, 100.0f);
    REQUIRE(moveList.getLatestMove()->getDeltaTime() == 99.0f);
}

TEST_CASE("MoveList::getLatestMove() returns the latest Move object in the MoveList") {
    MoveList moveList;

    moveList.addMove(InputState{}, 1);
    REQUIRE(moveList.getLatestMove()->getTimeStamp() == 1);

    moveList.addMove(InputState{}, 2);
    REQUIRE(moveList.getLatestMove()->getTimeStamp() == 2);

    moveList.addMove(InputState{}, 3);
    REQUIRE(moveList.getLatestMove()->getTimeStamp() == 3);

    moveList.removeMovesUntil(2);
    REQUIRE(moveList.getLatestMove()->getTimeStamp() == 3);
}

TEST_CASE("MoveList::getCount() returns the correct count of Move instances in the MoveList") {
    MoveList moveList;
    REQUIRE(moveList.getCount() == 0);

    moveList.addMove(InputState{}, 1);
    REQUIRE(moveList.getCount() == 1);

    moveList.addMove(InputState{}, 2);
    REQUIRE(moveList.getCount() == 2);

    moveList.addMove(InputState{}, 3);
    REQUIRE(moveList.getCount() == 3);

    moveList.removeMovesUntil(2);
    REQUIRE(moveList.getCount() == 1);
}

TEST_CASE("using MoveList::getLatestMove() throws an exception if the list empty") {
    MoveList moveList;
    REQUIRE_THROWS_AS(moveList.getLatestMove(), std::logic_error);
}

TEST_CASE("MoveList::clear() removes all Move objects from the MoveList") {
    MoveList moveList;
    moveList.addMove(InputState{}, 1);
    moveList.addMove(InputState{}, 2);
    moveList.addMove(InputState{}, 3);
    REQUIRE(moveList.getCount() == 3);
    moveList.clear();
    REQUIRE(moveList.getCount() == 0);
}

TEST_CASE("what is written with MoveList::write(...) can be read with MoveList::read(...)") {
    Packet packet(1500);
    MoveList moveList1, moveList2;

    moveList1.addMove(InputState{}, 1);
    moveList1.addMove(InputState{}, 2);
    moveList1.addMove(InputState{}, 3);
    REQUIRE(moveList1.getCount() == 3);
    moveList1.write(&packet);

    moveList2.read(&packet);
    REQUIRE(moveList2.getCount() == 3);
    REQUIRE(moveList2.getLatestMove()->getTimeStamp() == 3);
    REQUIRE(moveList2.getLatestMove()->getDeltaTime() == 1);
}
