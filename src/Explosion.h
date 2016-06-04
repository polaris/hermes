#ifndef _Explosion_H
#define _Explosion_H

#include "GameObject.h"
#include "Animation.h"
#include "Vector2d.h"

class Renderer;
class Packet;

class Explosion : public GameObject {
public:
    Explosion(const Renderer& renderer, const Vector2d& position);

    void update(float elapsed);

    void draw(Renderer& renderer);

    void write(Packet* packet);

    void read(Packet* packet);

    const Vector2d& getPosition() const;

    unsigned int getWidth() const;

    unsigned int getHeight() const;

    bool doesCollide() const;

    uint32_t getClassId() const;

    enum { ClassId = 3 };

protected:
    Animation animation_;

    Vector2d position_;
};

#endif  // _Explosion_H
