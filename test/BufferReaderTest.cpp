#include "BufferReader.h"
#include "catch.hpp"

TEST_CASE("BufferReader has correct default value", "[BufferReader]") {
    const std::size_t capacity = 32;

    Buffer buffer(capacity);
    BufferReader bufferReader(buffer);

    REQUIRE(bufferReader.capacity() == capacity);
    REQUIRE(bufferReader.position() == 0);
}

TEST_CASE("BufferReader can be read", "[BufferReader]") {
    const std::size_t capacity = 32;

    Buffer buffer(capacity);

    SECTION("Read one byte") {
        const uint8_t value = 0x12;
        buffer.write(&value, 0, sizeof(uint8_t));
        BufferReader bufferReader(buffer);
        uint8_t t{};
        bufferReader.read(t);
        REQUIRE(t == 0x12);
        REQUIRE(bufferReader.position() == 1);
    }

    SECTION("Read two bytes") {
        const uint16_t value = 0x3412;
        buffer.write(&value, 0, sizeof(uint16_t));
        BufferReader bufferReader(buffer);
        uint16_t t{};
        bufferReader.read(t);
        REQUIRE(t == 0x1234);
        REQUIRE(bufferReader.position() == 2);
    }

    SECTION("Read four bytes") {
        const uint32_t value = 0x78563412;
        buffer.write(&value, 0, sizeof(uint32_t));
        BufferReader bufferReader(buffer);
        uint32_t t{};
        bufferReader.read(t);
        REQUIRE(t == 0x12345678);
        REQUIRE(bufferReader.position() == 4);
    }

    SECTION("Read eight bytes") {
        const uint64_t value = 0xF0DEBC9A78563412;
        buffer.write(&value, 0, sizeof(uint64_t));
        BufferReader bufferReader(buffer);
        uint64_t t{};
        bufferReader.read(t);
        REQUIRE(t == 0x123456789ABCDEF0);
        REQUIRE(bufferReader.position() == 8);
    }
}
