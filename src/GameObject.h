#ifndef _GameObject_H
#define _GameObject_H

#include <memory>

class Renderer;
class Packet;
class Vector2d;

class GameObject {
public:
    GameObject();

    virtual ~GameObject() = default;

    bool checkCollision(GameObject* gameObject) const;

    void kill();

    virtual void update(float elapsed) = 0;

    virtual void draw(Renderer& renderer) = 0;

    virtual void write(Packet* packet) = 0;

    virtual void read(Packet* packet) = 0;

    virtual const Vector2d& getPosition() const = 0;

    virtual unsigned int getWidth() const = 0;

    virtual unsigned int getHeight() const = 0;

    virtual bool doesCollide() const;

    virtual bool dead() const;

    virtual uint32_t getClassId() const = 0;

private:
    bool dead_;
};

using GameObjectPtr = std::shared_ptr<GameObject>;

#endif  // _GameObject_H
