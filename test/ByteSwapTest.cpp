#include "ByteSwap.h"

#include <catch.hpp>

TEST_CASE("swap integral types", "[ByteSwap]") {
    REQUIRE(ByteSwap(uint8_t(0x12)) == 0x12);
    REQUIRE(ByteSwap(uint16_t(0x1234)) == 0x3412);
    REQUIRE(ByteSwap(uint32_t(0x12345678)) == 0x78563412);
    REQUIRE(ByteSwap(uint64_t(0x123456789ABCDEF0)) == 0xF0DEBC9A78563412);
}
