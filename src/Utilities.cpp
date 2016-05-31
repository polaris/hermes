#include "Utilities.h"

#include <cstdlib>

float randomValue(unsigned int max) {
    return static_cast<float>(std::rand() % static_cast<int>(max));
}

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}
