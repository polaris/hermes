#include "Utilities.h"

#include <cstdlib>

float randomValue(unsigned int max) {
    return static_cast<float>(std::rand() % static_cast<int>(max));
}
