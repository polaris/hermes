#include "Clock.h"

Clock::Clock()
: startTime(std::chrono::high_resolution_clock::now())
, lastTime(startTime)
, currentTime(startTime)
, elapsed(0) {
}

void Clock::update() {
    currentTime = std::chrono::high_resolution_clock::now();
    elapsed = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count()) / 1000.0f;
    lastTime = currentTime;
}

float Clock::getTime() const {
    const auto now = std::chrono::high_resolution_clock::now();
    return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count()) / 1000.0f;
}

float Clock::getElapsed() const {
    return elapsed;
}

float Clock::getGameTime() const {
    return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count()) / 1000.0f;
}

float Clock::getFrameDuration() const {
    const auto now = std::chrono::high_resolution_clock::now();
    return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - currentTime).count()) / 1000.0f;
}
