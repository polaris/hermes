#ifndef _GameObject_H
#define _GameObject_H

class Renderer;

class GameObject {
public:
    virtual ~GameObject() = default;

    virtual void update(float elapsed) = 0;

    virtual void draw(const Renderer &renderer) = 0;
};

#endif  // _GameObject_H
