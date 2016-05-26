#ifndef _LaserBolt_H
#define _LaserBolt_H

#include "GameObject.h"
#include "Sprite.h"
#include "Vector2d.h"

class Renderer;
class Packet;

class LaserBolt : public GameObject {
public:
    LaserBolt(const Renderer& renderer, const Vector2d& position, const Vector2d& velocity);

    virtual void update(float elapsed) override;

    void draw(Renderer &renderer) override;

    void write(Packet* packet) override;

    virtual void read(Packet* packet) override;

    const Vector2d& getPosition() const override;

    uint32_t getClassId() const override;

    enum { ClassId = 2 };

private:
    Sprite sprite_;

    Vector2d position_;

    Vector2d velocity_;

    Vector2d lookat_;
};

#endif  // _LaserBolt_H
