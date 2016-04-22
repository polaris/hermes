#include "Queue.h"
#include "Packet.h"
#include "catch.hpp"

TEST_CASE("An empty queue can be constructed", "[Queue]") {
    const std::size_t capacity = 8;
    Queue<Packet> queue(capacity);
    REQUIRE(queue.pop() == nullptr);

    queue.push(new Packet(32));
    auto packet = queue.pop();
    REQUIRE(packet != nullptr);
    delete packet;
}

TEST_CASE("Pushing and popping of a queue", "[Queue]") {
    const std::size_t capacity = 4;

    Queue<Packet> pool(capacity);
    for (std::size_t i = 0; i < capacity; ++i) {
        pool.push(new Packet(32));
    }
    Queue<Packet> queue(capacity);

    Packet *packet{nullptr};

    for (std::size_t i = 0; i < capacity; i++) {
        packet = pool.pop();
        REQUIRE(packet != nullptr);
        queue.push(packet);
    }
    REQUIRE(pool.pop() == nullptr);

    for (std::size_t i = 0; i < capacity; i++) {
        packet = queue.pop();
        REQUIRE(packet != nullptr);
        pool.push(packet);
    }
    REQUIRE(queue.pop() == nullptr);
}
