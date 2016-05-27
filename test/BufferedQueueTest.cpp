#include "BufferedQueue.h"

#include <catch.hpp>

TEST_CASE("pop returns a valid packet pointer or nullptr if the pool is empty") {
    BufferedQueue bufferedQueue(4);

    Packet* packet = bufferedQueue.pop();
    REQUIRE(packet != nullptr);
    bufferedQueue.enqueue(packet);

    packet = bufferedQueue.pop();
    REQUIRE(packet != nullptr);
    bufferedQueue.enqueue(packet);

    packet = bufferedQueue.pop();
    REQUIRE(packet != nullptr);
    bufferedQueue.enqueue(packet);

    packet = bufferedQueue.pop();
    REQUIRE(packet != nullptr);
    bufferedQueue.enqueue(packet);

    packet = bufferedQueue.pop();
    REQUIRE(packet == nullptr);
}

TEST_CASE("dequeue returns a valid packet pointer or nullptr if the queue is empty") {
    BufferedQueue bufferedQueue(4);

    bufferedQueue.enqueue(bufferedQueue.pop());
    bufferedQueue.enqueue(bufferedQueue.pop());
    bufferedQueue.enqueue(bufferedQueue.pop());
    bufferedQueue.enqueue(bufferedQueue.pop());

    Packet* packet = bufferedQueue.dequeue();
    REQUIRE(packet != nullptr);
    bufferedQueue.push(packet);

    packet = bufferedQueue.dequeue();
    REQUIRE(packet != nullptr);
    bufferedQueue.push(packet);

    packet = bufferedQueue.dequeue();
    REQUIRE(packet != nullptr);
    bufferedQueue.push(packet);

    packet = bufferedQueue.dequeue();
    REQUIRE(packet != nullptr);
    bufferedQueue.push(packet);

    packet = bufferedQueue.dequeue();
    REQUIRE(packet == nullptr);
}


TEST_CASE("getNumEnqueued returns the number of enqueued packets") {
    BufferedQueue bufferedQueue(4);
    REQUIRE(bufferedQueue.getNumEnqueued() == 0);

    bufferedQueue.enqueue(bufferedQueue.pop());
    REQUIRE(bufferedQueue.getNumEnqueued() == 1);

    bufferedQueue.enqueue(bufferedQueue.pop());
    REQUIRE(bufferedQueue.getNumEnqueued() == 2);

    bufferedQueue.enqueue(bufferedQueue.pop());
    REQUIRE(bufferedQueue.getNumEnqueued() == 3);

    bufferedQueue.enqueue(bufferedQueue.pop());
    REQUIRE(bufferedQueue.getNumEnqueued() == 4);

    bufferedQueue.push(bufferedQueue.dequeue());
    REQUIRE(bufferedQueue.getNumEnqueued() == 3);

    bufferedQueue.push(bufferedQueue.dequeue());
    REQUIRE(bufferedQueue.getNumEnqueued() == 2);

    bufferedQueue.push(bufferedQueue.dequeue());
    REQUIRE(bufferedQueue.getNumEnqueued() == 1);

    bufferedQueue.push(bufferedQueue.dequeue());
    REQUIRE(bufferedQueue.getNumEnqueued() == 0);
}
