#ifndef _ServerWorld_H
#define _ServerWorld_H

#include "World.h"

class ServerWorld : public World {
public:
    ServerWorld(unsigned int width, unsigned int height);

    void update(float elapsed) override;

private:
    const unsigned int width_;
    const unsigned int height_;
};

#endif  // _ServerWorld_H
