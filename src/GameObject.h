#ifndef _GameObject_H
#define _GameObject_H

#include <memory>

class Renderer;

class GameObject {
public:
    virtual ~GameObject() = default;

    virtual void update(float elapsed) = 0;

    virtual void draw(Renderer &renderer) = 0;
};

using GameObjectPtr = std::shared_ptr<GameObject>;

#endif  // _GameObject_H
