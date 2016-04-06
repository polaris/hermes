#include "Queue.h"
#include "Buffer.h"
#include "catch.hpp"

TEST_CASE("Queue can be constructed", "[Queue]") {
    const std::size_t capacity = 8;
    Queue<Buffer<32>> queue(capacity);
}
