#include "Buffer.h"
#include "catch.hpp"

TEST_CASE("Buffer has correct default values", "[Buffer]") {
    Buffer buffer(8);
    REQUIRE(buffer.capacity() == 8);
}

TEST_CASE("Buffer can be read and written", "[Buffer]") {
    Buffer buffer(8);

    const uint32_t value = 0x12345678;
    REQUIRE_NOTHROW(buffer.write(reinterpret_cast<const uint8_t*>(&value) + 0, sizeof(uint8_t)));
    REQUIRE_NOTHROW(buffer.write(reinterpret_cast<const uint8_t*>(&value) + 1, sizeof(uint8_t)));
    REQUIRE_NOTHROW(buffer.write(reinterpret_cast<const uint8_t*>(&value) + 2, sizeof(uint8_t)));
    REQUIRE_NOTHROW(buffer.write(reinterpret_cast<const uint8_t*>(&value) + 3, sizeof(uint8_t)));

    buffer.reset();

    uint32_t output{};
    REQUIRE_NOTHROW(buffer.read(&output, sizeof(uint32_t)));

    REQUIRE(output == value);
}

TEST_CASE("Exception is thrown when writing out of bounds", "[Buffer]") {
    Buffer buffer(2);

    uint32_t value = 0;
    REQUIRE_THROWS_AS(buffer.write(&value, sizeof(uint32_t)), std::out_of_range);
}

TEST_CASE("Size can be set", "[Buffer]") {
    Buffer buffer(2);

    REQUIRE(buffer.size() == 0);
    REQUIRE_NOTHROW(buffer.size(1));
    REQUIRE(buffer.size() == 1);
    REQUIRE_NOTHROW(buffer.size(2));
    REQUIRE(buffer.size() == 2);
    REQUIRE_THROWS_AS(buffer.size(3), std::out_of_range);
}

TEST_CASE("BufferReader can be read", "[BufferReader]") {
    Buffer buffer(32);

    SECTION("Read one byte") {
        const uint8_t value = 0x12;
        buffer.write(&value, sizeof(uint8_t));
        buffer.reset();
        uint8_t t{};
        buffer.read(t);
        REQUIRE(t == 0x12);
        REQUIRE(buffer.size() == 1);
    }

    SECTION("Read two bytes") {
        const uint16_t value = 0x3412;
        buffer.write(&value, sizeof(uint16_t));
        buffer.reset();
        uint16_t t{};
        buffer.read(t);
        REQUIRE(t == 0x1234);
        REQUIRE(buffer.size() == 2);
    }

    SECTION("Read four bytes") {
        const uint32_t value = 0x78563412;
        buffer.write(&value, sizeof(uint32_t));
        buffer.reset();
        uint32_t t{};
        buffer.read(t);
        REQUIRE(t == 0x12345678);
        REQUIRE(buffer.size() == 4);
    }

    SECTION("Read eight bytes") {
        const uint64_t value = 0xF0DEBC9A78563412;
        buffer.write(&value, sizeof(uint64_t));
        buffer.reset();
        uint64_t t{};
        buffer.read(t);
        REQUIRE(t == 0x123456789ABCDEF0);
        REQUIRE(buffer.size() == 8);
    }
}

TEST_CASE("Buffer has correct default values 2", "[Buffer]") {
    Buffer buffer(32);
    REQUIRE(buffer.capacity() == 32);
    REQUIRE(buffer.size() == 0);
}

TEST_CASE("Buffer can be cleared", "[Buffer]") {
    Buffer buffer(32);

    const uint8_t value = 0x12;
    buffer.write(value);

    REQUIRE(*buffer.data() == value);
    REQUIRE(buffer.size() == 1);

    buffer.clear();
    buffer.reset();

    REQUIRE(*buffer.data() == 0x00);
    REQUIRE(buffer.size() == 0);
}

TEST_CASE("Buffer can be written", "[Buffer]") {
    Buffer buffer(32);

    SECTION("Write single byte") {
        const uint8_t value = 0x12;
        buffer.write(value);
        REQUIRE(buffer.size() == 1);
        REQUIRE(*buffer.data() == 0x12);
    }

    SECTION("Write two bytes") {
        const uint16_t value = 0x1234;
        buffer.write(value);
        REQUIRE(buffer.size() == 2);
        REQUIRE(*(reinterpret_cast<const uint16_t*>(buffer.data())) == 0x3412);
    }

    SECTION("Write four bytes") {
        const uint32_t value = 0x12345678;
        buffer.write(value);
        REQUIRE(buffer.size() == 4);
        REQUIRE(*(reinterpret_cast<const uint32_t*>(buffer.data())) == 0x78563412);
    }

    SECTION("Write eight bytes") {
        const uint64_t value = 0x123456789ABCDEF0;
        buffer.write(value);
        REQUIRE(buffer.size() == 8);
        REQUIRE(*(reinterpret_cast<const uint64_t*>(buffer.data())) == 0xF0DEBC9A78563412);
    }
}
