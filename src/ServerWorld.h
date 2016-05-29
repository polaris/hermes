#ifndef _ServerWorld_H
#define _ServerWorld_H

#include "World.h"

#include <functional>

using ConfirmCollisionFunc = std::function<bool(uint32_t, uint32_t)>;
using RemovedObjectFunc = std::function<void(uint32_t)>;

class ServerWorld : public World {
public:
    ServerWorld(unsigned int width, unsigned int height, ConfirmCollisionFunc confirmCollisionFunc, RemovedObjectFunc removedObjectFunc);

    void update(float elapsed) override;

private:
    const unsigned int width_;
    const unsigned int height_;

    ConfirmCollisionFunc confirmCollisionFunc_;
    RemovedObjectFunc removedObjectFunc_;
};

#endif  // _ServerWorld_H
