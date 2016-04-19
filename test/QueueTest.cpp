#include "Queue.h"
#include "Buffer.h"
#include "catch.hpp"

TEST_CASE("An empty queue can be constructed", "[Queue]") {
    const std::size_t capacity = 8;
    Queue<Buffer> queue(capacity);
    REQUIRE(queue.pop() == nullptr);

    queue.push(new Buffer(32));
    auto buffer = queue.pop();
    REQUIRE(buffer != nullptr);
    delete buffer;
}

TEST_CASE("Pushing and popping of a queue", "[Queue]") {
    const std::size_t capacity = 4;

    Queue<Buffer> pool(capacity);
    for (std::size_t i = 0; i < capacity; ++i) {
        pool.push(new Buffer(32));
    }
    Queue<Buffer> queue(capacity);

    Buffer *buffer{nullptr};

    for (std::size_t i = 0; i < capacity; i++) {
        buffer = pool.pop();
        REQUIRE(buffer != nullptr);
        queue.push(buffer);
    }
    REQUIRE(pool.pop() == nullptr);

    for (std::size_t i = 0; i < capacity; i++) {
        buffer = queue.pop();
        REQUIRE(buffer != nullptr);
        pool.push(buffer);
    }
    REQUIRE(queue.pop() == nullptr);
}
