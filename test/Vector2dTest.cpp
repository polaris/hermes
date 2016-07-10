#include "Vector2d.h"
#include "Packet.h"

#include <catch.hpp>

TEST_CASE("Vector2d default construction") {
    const Vector2d v;
    REQUIRE(v.getX() == 0.0f);
    REQUIRE(v.getY() == 0.0f);
    REQUIRE(v.length() == 0.0f);
}

TEST_CASE("Vector2d construction with coordinates") {
    const Vector2d v{2.0f, 0.0f};
    REQUIRE(v.getX() == 2.0f);
    REQUIRE(v.getY() == 0.0f);
    REQUIRE(v.length() == 2.0f);
}

TEST_CASE("Vector2d copy construction") {
    const Vector2d a{1.0f, 2.0f};
    const Vector2d b{a};
    REQUIRE(a.getX() == a.getX());
    REQUIRE(a.getY() == b.getY());
}

TEST_CASE("Vector2d assignment operator") {
    const Vector2d a{2, 3};
    Vector2d b;
    b = a;
    REQUIRE(a.getX() == a.getX());
    REQUIRE(a.getY() == b.getY());
}

TEST_CASE("Vector2d addition operator 1") {
    const Vector2d a{2, 3};
    const Vector2d b{123, 456};
    const Vector2d c = b + a;
    REQUIRE(c.getX() == 125);
    REQUIRE(c.getY() == 459);
}

TEST_CASE("Vector2d addition operator 2") {
    const Vector2d a{2, 3};
    Vector2d b{123, 456};
    b += a;
    REQUIRE(b.getX() == 125);
    REQUIRE(b.getY() == 459);
}

TEST_CASE("Vector2d subtraction operator 1") {
    const Vector2d a{2, 3};
    const Vector2d b{123, 456};
    const Vector2d c = b - a;
    REQUIRE(c.getX() == 121);
    REQUIRE(c.getY() == 453);
}

TEST_CASE("Vector2d subtraction operator 2") {
    const Vector2d a{2, 3};
    Vector2d b{123, 456};
    b -= a;
    REQUIRE(b.getX() == 121);
    REQUIRE(b.getY() == 453);
}

TEST_CASE("Vector2d scalar multiplication 1") {
    const Vector2d a{2, 4};
    const Vector2d b = 5 * a;
    REQUIRE(b.getX() == 10);
    REQUIRE(b.getY() == 20);
}

TEST_CASE("Vector2d scalar multiplication 2") {
    Vector2d a{2, 4};
    a *= 5;
    REQUIRE(a == Vector2d(10, 20));
}

TEST_CASE("Vector2d angles") {
    REQUIRE(angle(Vector2d{0, 1}, Vector2d{ 0,  1}) == Approx( 0         ).epsilon(0.000001));
    REQUIRE(angle(Vector2d{0, 1}, Vector2d{ 0, -1}) == Approx( M_PI      ).epsilon(0.000001));
    REQUIRE(angle(Vector2d{0, 1}, Vector2d{ 1,  0}) == Approx( M_PI / 2.0).epsilon(0.000001));
    REQUIRE(angle(Vector2d{0, 1}, Vector2d{-1,  0}) == Approx(-M_PI / 2.0).epsilon(0.000001));
}

TEST_CASE("Vector2d rotation 1") {
    Vector2d a{1, 0};
    const Vector2d b{-1, 0};
    a.rotate(static_cast<float>(M_PI));
    REQUIRE(a.getX() == Approx(b.getX()).epsilon(0.000001));
    REQUIRE(a.getY() == Approx(b.getY()).epsilon(0.000001));
}

TEST_CASE("Vector2d rotation 2") {
    Vector2d a{1, 0};
    const Vector2d b{0, 1};
    a.rotate(static_cast<float>(M_PI / 2.0));
    REQUIRE(a.getX() == Approx(b.getX()).epsilon(0.000001));
    REQUIRE(a.getY() == Approx(b.getY()).epsilon(0.000001));
}

TEST_CASE("Vector2d rotation 3") {
    Vector2d a{1, 0};
    const Vector2d b{0, -1};
    a.rotate(static_cast<float>(-M_PI / 2.0));
    REQUIRE(a.getX() == Approx(b.getX()).epsilon(0.000001));
    REQUIRE(a.getY() == Approx(b.getY()).epsilon(0.000001));
}

TEST_CASE("Vector2d rotation 4") {
    Vector2d a{1, 0};
    const Vector2d b{1, 0};
    a.rotate(static_cast<float>(M_PI * 2.0));
    REQUIRE(a.getX() == Approx(b.getX()).epsilon(0.000001));
    REQUIRE(a.getY() == Approx(b.getY()).epsilon(0.000001));
}

TEST_CASE("Vector2d serialization") {
    const Vector2d a{123.456f, 789.456f};
    Vector2d b;

    Packet packet(32);
    a.write(&packet);
    packet.reset();
    b.read(&packet);

    REQUIRE(a == b);
}

TEST_CASE("Vector2d linear interpolation") {
    const Vector2d a{ 5.0f, 15.0f};
    const Vector2d b{10.0f, 45.0f};
    REQUIRE(lerp(a, b, 0.0f) == a);
    REQUIRE(lerp(a, b, 0.5f) == Vector2d(7.5f, 30.0f));
    REQUIRE(lerp(a, b, 1.0f) == b);
}
