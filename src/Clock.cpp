#include "Clock.h"

Clock::Clock()
: startTime(std::chrono::high_resolution_clock::now())
, lastTime(startTime)
, currentTime(startTime)
, elapsed(0) {
}

void Clock::update() {
    currentTime = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count() / 1000.0f;
    lastTime = currentTime;
}

float Clock::getElapsed() const {
    return elapsed;
}

float Clock::getGameTime() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;
}

float Clock::getFrameDuration() const {
    const auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - currentTime).count() / 1000.0f;
}
