#include "Window.h"
#include "catch.hpp"

TEST_CASE("create a window") {
    const unsigned int width = 640;
    const unsigned int height = 480;
    Window window("Test", width, height);
    REQUIRE(width == window.getWidth());
    REQUIRE(height == window.getHeight());
}