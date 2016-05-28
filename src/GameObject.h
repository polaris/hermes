#ifndef _GameObject_H
#define _GameObject_H

#include <memory>

class Renderer;
class Packet;
class Vector2d;

class GameObject {
public:
    virtual ~GameObject() = default;

    bool checkCollision(GameObject* gameObject) const;

    virtual void update(float elapsed) = 0;

    virtual void draw(Renderer& renderer) = 0;

    virtual void write(Packet* packet) = 0;

    virtual void read(Packet* packet) = 0;

    virtual const Vector2d& getPosition() const = 0;

    virtual unsigned int getWidth() const = 0;

    virtual unsigned int getHeight() const = 0;

    virtual uint32_t getClassId() const = 0;
};

using GameObjectPtr = std::shared_ptr<GameObject>;

#endif  // _GameObject_H
