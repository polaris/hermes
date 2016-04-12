#include "Vector2d.h"
#include "catch.hpp"

TEST_CASE("Vector2d default construction") {
    Vector2d v;
    REQUIRE(v.x() == 0.0f);
    REQUIRE(v.y() == 0.0f);
    REQUIRE(v.length() == 0.0f);
}

TEST_CASE("Vector2d construction with coordinates") {
    Vector2d v{2.0f, 0.0f};
    REQUIRE(v.x() == 2.0f);
    REQUIRE(v.y() == 0.0f);
    REQUIRE(v.length() == 2.0f);
}

TEST_CASE("Vector2d copy construction") {
    Vector2d a{1.0f, 2.0f};
    Vector2d b{a};
    REQUIRE(a.x() == a.x());
    REQUIRE(a.y() == b.y());
}

TEST_CASE("Vector2d assignment operator") {
    Vector2d a{2, 3};
    Vector2d b;
    b = a;
    REQUIRE(a.x() == a.x());
    REQUIRE(a.y() == b.y());
}
