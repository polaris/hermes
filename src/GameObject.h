#ifndef _GameObject_H
#define _GameObject_H

class GameObject {
public:
    virtual ~GameObject() = default;

    virtual void update() = 0;

    virtual void draw() = 0;
};

#endif  // _GameObject_H
