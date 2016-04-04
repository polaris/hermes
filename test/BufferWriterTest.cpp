#include "BufferWriter.h"
#include "catch.hpp"

TEST_CASE("BufferWriter has correct defaults", "[BufferWriter]") {
    const std::size_t capacity = 32;

    BufferWriter bufferWriter(capacity);

    REQUIRE(bufferWriter.capacity() == capacity);
    REQUIRE(bufferWriter.size() == 0);
}

TEST_CASE("BufferWriter can be written", "[BufferWriter]") {
    const std::size_t capacity = 32;

    BufferWriter bufferWriter(capacity);

    SECTION("Write single byte") {
        uint8_t value = 0x12;
        bufferWriter.write(value);
        REQUIRE(bufferWriter.size() == 1);
        REQUIRE(*bufferWriter.data() == 0x12);
    }

    SECTION("Write two bytes") {
        uint16_t value = 0x1234;
        bufferWriter.write(value);
        REQUIRE(bufferWriter.size() == 2);
        REQUIRE(*(reinterpret_cast<const uint16_t*>(bufferWriter.data())) == 0x3412);
    }

    SECTION("Write four bytes") {
        uint32_t value = 0x12345678;
        bufferWriter.write(value);
        REQUIRE(bufferWriter.size() == 4);
        REQUIRE(*(reinterpret_cast<const uint32_t*>(bufferWriter.data())) == 0x78563412);
    }

    SECTION("Write eight bytes") {
        uint64_t value = 0x123456789ABCDEF0;
        bufferWriter.write(value);
        REQUIRE(bufferWriter.size() == 8);
        REQUIRE(*(reinterpret_cast<const uint64_t*>(bufferWriter.data())) == 0xF0DEBC9A78563412);
    }
}
