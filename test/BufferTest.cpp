#include "Buffer.h"
#include "catch.hpp"

TEST_CASE("Buffer has correct default values", "[Buffer]") {
    Buffer<8> buffer;
    REQUIRE(buffer.capacity() == 8);
}

/*TEST_CASE("Buffer can be read and written", "[Buffer]") {
    Buffer<8> buffer;

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
    Buffer<4> buffer;

    uint32_t value = 0;
    REQUIRE_THROWS_AS(buffer.write(&value, 1, sizeof(uint32_t)), std::out_of_range);
    REQUIRE_THROWS_AS(buffer.write(&value, 2, sizeof(uint32_t)), std::out_of_range);
    REQUIRE_THROWS_AS(buffer.write(&value, 3, sizeof(uint32_t)), std::out_of_range);
}*/

TEST_CASE("BufferReader can be read", "[BufferReader]") {
    Buffer<32> buffer;

    SECTION("Read one byte") {
        const uint8_t value = 0x12;
        buffer.write(&value, sizeof(uint8_t));
        buffer.reset();
        uint8_t t{};
        buffer.read(t);
        REQUIRE(t == 0x12);
        REQUIRE(buffer.position() == 1);
    }

    SECTION("Read two bytes") {
        const uint16_t value = 0x3412;
        buffer.write(&value, sizeof(uint16_t));
        buffer.reset();
        uint16_t t{};
        buffer.read(t);
        REQUIRE(t == 0x1234);
        REQUIRE(buffer.position() == 2);
    }

    SECTION("Read four bytes") {
        const uint32_t value = 0x78563412;
        buffer.write(&value, sizeof(uint32_t));
        buffer.reset();
        uint32_t t{};
        buffer.read(t);
        REQUIRE(t == 0x12345678);
        REQUIRE(buffer.position() == 4);
    }

    SECTION("Read eight bytes") {
        const uint64_t value = 0xF0DEBC9A78563412;
        buffer.write(&value, sizeof(uint64_t));
        buffer.reset();
        uint64_t t{};
        buffer.read(t);
        REQUIRE(t == 0x123456789ABCDEF0);
        REQUIRE(buffer.position() == 8);
    }
}

TEST_CASE("Buffer has correct default values 2", "[Buffer]") {
    Buffer<32> buffer;
    REQUIRE(buffer.capacity() == 32);
    REQUIRE(buffer.position() == 0);
}

TEST_CASE("Buffer can be cleared", "[Buffer]") {
    Buffer<32> buffer;

    const uint8_t value = 0x12;
    buffer.write(value);

    REQUIRE(*buffer.data() == value);
    REQUIRE(buffer.position() == 1);

    buffer.clear();
    buffer.reset();

    REQUIRE(*buffer.data() == 0x00);
    REQUIRE(buffer.position() == 0);
}

TEST_CASE("Buffer can be written", "[Buffer]") {
    Buffer<32> buffer;

    SECTION("Write single byte") {
        const uint8_t value = 0x12;
        buffer.write(value);
        REQUIRE(buffer.position() == 1);
        REQUIRE(*buffer.data() == 0x12);
    }

    SECTION("Write two bytes") {
        const uint16_t value = 0x1234;
        buffer.write(value);
        REQUIRE(buffer.position() == 2);
        REQUIRE(*(reinterpret_cast<const uint16_t*>(buffer.data())) == 0x3412);
    }

    SECTION("Write four bytes") {
        const uint32_t value = 0x12345678;
        buffer.write(value);
        REQUIRE(buffer.position() == 4);
        REQUIRE(*(reinterpret_cast<const uint32_t*>(buffer.data())) == 0x78563412);
    }

    SECTION("Write eight bytes") {
        const uint64_t value = 0x123456789ABCDEF0;
        buffer.write(value);
        REQUIRE(buffer.position() == 8);
        REQUIRE(*(reinterpret_cast<const uint64_t*>(buffer.data())) == 0xF0DEBC9A78563412);
    }
}
