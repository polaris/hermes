#include "Clock.h"

#include <catch.hpp>

#include <thread>

TEST_CASE("No time elapsed after construction") {
    Clock clock;
    REQUIRE(clock.getElapsed() == 0.0f);
}

TEST_CASE("Elapsed time is properly updated") {
    Clock clock;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    clock.update();
    REQUIRE(clock.getElapsed() >= 0.1f);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    clock.update();
    REQUIRE(clock.getElapsed() >= 0.5f);
}

TEST_CASE("Elapsed time is not implicitly updated") {
    Clock clock;

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    clock.update();
    REQUIRE(clock.getElapsed() >= 0.2f);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    REQUIRE(clock.getElapsed() >= 0.2f);
}

TEST_CASE("Game time is at zero after construction") {
    Clock clock;
    REQUIRE(clock.getGameTime() == 0.0f);
}

TEST_CASE("Game time is properly updated") {
    Clock clock;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    clock.update();
    REQUIRE(clock.getGameTime() >= 0.1f);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    clock.update();
    REQUIRE(clock.getGameTime() >= 0.6f);
}

TEST_CASE("Game time is not implicitly updated") {
    Clock clock;

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    clock.update();
    REQUIRE(clock.getGameTime() >= 0.2f);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    REQUIRE(clock.getGameTime() >= 0.2f);
}

TEST_CASE("Frame time is approximately zero after construction") {
    Clock clock;
    REQUIRE(clock.getFrameTime() == Approx(0.0f).epsilon(0.000001f));
}

TEST_CASE("Frame time is properly updated") {
    Clock clock;
 
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    REQUIRE(clock.getFrameTime() == Approx(0.5f).epsilon(0.000001f));

    clock.update();
    REQUIRE(clock.getFrameTime() == Approx(0.0f).epsilon(0.000001f));
}

TEST_CASE("Current time is zero after construction") {
    Clock clock;
    REQUIRE(clock.getTime() == Approx(0.0f).epsilon(0.000001f));
}

TEST_CASE("Current time is updated implicitly") {
    Clock clock;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    REQUIRE(clock.getTime() == Approx(0.1f).epsilon(0.000001f));

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    REQUIRE(clock.getTime() == Approx(0.3f).epsilon(0.000001f));
}

TEST_CASE("Current time is not affected by updates") {
    Clock clock;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    REQUIRE(clock.getTime() == Approx(0.1f).epsilon(0.000001f));

    clock.update();

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    REQUIRE(clock.getTime() == Approx(0.3f).epsilon(0.000001f));
}
