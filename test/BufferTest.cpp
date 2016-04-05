#include "Buffer.h"
#include "catch.hpp"

TEST_CASE("Buffer has correct default values", "[Buffer]") {
    const std::size_t capacity = 8;
    Buffer buffer(capacity);
    REQUIRE(buffer.capacity() == capacity);
}

TEST_CASE("Buffer can be read and written", "[Buffer]") {
    const std::size_t capacity = 8;
    Buffer buffer(capacity);

    const uint64_t value = 0x12345678;
    buffer.write(reinterpret_cast<const uint8_t*>(&value) + 0, 0, sizeof(uint8_t));
    buffer.write(reinterpret_cast<const uint8_t*>(&value) + 1, 1, sizeof(uint8_t));
    buffer.write(reinterpret_cast<const uint8_t*>(&value) + 2, 2, sizeof(uint8_t));
    buffer.write(reinterpret_cast<const uint8_t*>(&value) + 3, 3, sizeof(uint8_t));

    uint64_t output{};
    buffer.read(&output, 0, sizeof(uint64_t));

    REQUIRE(output == value);
}
