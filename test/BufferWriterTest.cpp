#include "BufferWriter.h"
#include "catch.hpp"

TEST_CASE("BufferWriter has correct default values", "[BufferWriter]") {
    const std::size_t capacity = 32;

    Buffer buffer(capacity);
    BufferWriter bufferWriter(buffer);

    REQUIRE(bufferWriter.capacity() == capacity);
    REQUIRE(bufferWriter.position() == 0);
}

TEST_CASE("BufferWriter can be cleared", "[BufferWriter]") {
    const std::size_t capacity = 32;

    Buffer buffer(capacity);
    BufferWriter bufferWriter(buffer);

    const uint8_t value = 0x12;
    bufferWriter.write(value);

    REQUIRE(*buffer.data() == value);
    REQUIRE(bufferWriter.position() == 1);

    bufferWriter.clear();

    REQUIRE(*buffer.data() == 0x00);
    REQUIRE(bufferWriter.position() == 0);
}

TEST_CASE("BufferWriter can be written", "[BufferWriter]") {
    const std::size_t capacity = 32;

    Buffer buffer(capacity);
    BufferWriter bufferWriter(buffer);

    SECTION("Write single byte") {
        const uint8_t value = 0x12;
        bufferWriter.write(value);
        REQUIRE(bufferWriter.position() == 1);
        REQUIRE(*buffer.data() == 0x12);
    }

    SECTION("Write two bytes") {
        const uint16_t value = 0x1234;
        bufferWriter.write(value);
        REQUIRE(bufferWriter.position() == 2);
        REQUIRE(*(reinterpret_cast<const uint16_t*>(buffer.data())) == 0x3412);
    }

    SECTION("Write four bytes") {
        const uint32_t value = 0x12345678;
        bufferWriter.write(value);
        REQUIRE(bufferWriter.position() == 4);
        REQUIRE(*(reinterpret_cast<const uint32_t*>(buffer.data())) == 0x78563412);
    }

    SECTION("Write eight bytes") {
        const uint64_t value = 0x123456789ABCDEF0;
        bufferWriter.write(value);
        REQUIRE(bufferWriter.position() == 8);
        REQUIRE(*(reinterpret_cast<const uint64_t*>(buffer.data())) == 0xF0DEBC9A78563412);
    }
}
