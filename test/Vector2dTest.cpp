#include "Vector2d.h"
#include "catch.hpp"

TEST_CASE("Vector2d default construction") {
    const Vector2d v;
    REQUIRE(v.x() == 0.0f);
    REQUIRE(v.y() == 0.0f);
    REQUIRE(v.length() == 0.0f);
}

TEST_CASE("Vector2d construction with coordinates") {
    const Vector2d v{2.0f, 0.0f};
    REQUIRE(v.x() == 2.0f);
    REQUIRE(v.y() == 0.0f);
    REQUIRE(v.length() == 2.0f);
}

TEST_CASE("Vector2d copy construction") {
    const Vector2d a{1.0f, 2.0f};
    const Vector2d b{a};
    REQUIRE(a.x() == a.x());
    REQUIRE(a.y() == b.y());
}

TEST_CASE("Vector2d assignment operator") {
    const Vector2d a{2, 3};
    Vector2d b;
    b = a;
    REQUIRE(a.x() == a.x());
    REQUIRE(a.y() == b.y());
}

TEST_CASE("Vector2d addition operator 1") {
    const Vector2d a{2, 3};
    const Vector2d b{123, 456};
    const Vector2d c = b + a;
    REQUIRE(c.x() == 125);
    REQUIRE(c.y() == 459);
}

TEST_CASE("Vector2d addition operator 2") {
    const Vector2d a{2, 3};
    Vector2d b{123, 456};
    b += a;
    REQUIRE(b.x() == 125);
    REQUIRE(b.y() == 459);
}

TEST_CASE("Vector2d subtraction operator 1") {
    const Vector2d a{2, 3};
    const Vector2d b{123, 456};
    const Vector2d c = b - a;
    REQUIRE(c.x() == 121);
    REQUIRE(c.y() == 453);
}

TEST_CASE("Vector2d subtraction operator 2") {
    const Vector2d a{2, 3};
    Vector2d b{123, 456};
    b -= a;
    REQUIRE(b.x() == 121);
    REQUIRE(b.y() == 453);
}

TEST_CASE("Vector2d scalar multiplication 1") {
    const Vector2d a{2, 4};
    const Vector2d b = 5 * a;
    REQUIRE(b.x() == 10);
    REQUIRE(b.y() == 20);
}

TEST_CASE("Vector2d scalar multiplication 2") {
    Vector2d a{2, 4};
    a *= 5;
    REQUIRE(a.x() == 10);
    REQUIRE(a.y() == 20);
}
