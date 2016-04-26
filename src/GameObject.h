#ifndef _GameObject_H
#define _GameObject_H

#include <memory>

class Renderer;
class Packet;

class GameObject {
public:
    virtual ~GameObject() = default;

    virtual void update(float elapsed) = 0;

    virtual void draw(Renderer &renderer) = 0;

    virtual void write(Packet* packet) = 0;

    virtual void read(Packet* packet) = 0;

    virtual unsigned int getClassId() const = 0;
};

using GameObjectPtr = std::shared_ptr<GameObject>;

#endif  // _GameObject_H
