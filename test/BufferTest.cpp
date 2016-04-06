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

    const uint32_t value = 0x12345678;
    REQUIRE_NOTHROW(buffer.write(reinterpret_cast<const uint8_t*>(&value) + 0, 0, sizeof(uint8_t)));
    REQUIRE_NOTHROW(buffer.write(reinterpret_cast<const uint8_t*>(&value) + 1, 1, sizeof(uint8_t)));
    REQUIRE_NOTHROW(buffer.write(reinterpret_cast<const uint8_t*>(&value) + 2, 2, sizeof(uint8_t)));
    REQUIRE_NOTHROW(buffer.write(reinterpret_cast<const uint8_t*>(&value) + 3, 3, sizeof(uint8_t)));

    uint32_t output{};
    REQUIRE_NOTHROW(buffer.read(&output, 0, sizeof(uint32_t)));

    REQUIRE(output == value);
}

TEST_CASE("Exception is thrown when writing out of bounds", "[Buffer]") {
    const std::size_t capacity = 4;
    Buffer buffer(capacity);

    uint32_t value = 0;
    REQUIRE_THROWS_AS(buffer.write(&value, 1, sizeof(uint32_t)), std::out_of_range);
    REQUIRE_THROWS_AS(buffer.write(&value, 2, sizeof(uint32_t)), std::out_of_range);
    REQUIRE_THROWS_AS(buffer.write(&value, 3, sizeof(uint32_t)), std::out_of_range);
}