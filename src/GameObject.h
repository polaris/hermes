#ifndef _GameObject_H
#define _GameObject_H

#include <memory>

class Renderer;
class Packet;

class GameObject {
public:
    GameObject()
    : dirty_(false) {
    }

    virtual ~GameObject() = default;

    bool isDirty() const {
        return dirty_;
    }

    void setDirty() {
        dirty_ = true;
    }

    void resetDirty() {
        dirty_ = false;
    }

    virtual void update(float elapsed) = 0;

    virtual void draw(Renderer &renderer) = 0;

    virtual void write(Packet* packet) = 0;

    virtual void read(Packet* packet) = 0;

    virtual uint32_t getClassId() const = 0;

private:
    bool dirty_;
};

using GameObjectPtr = std::shared_ptr<GameObject>;

#endif  // _GameObject_H
