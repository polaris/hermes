#ifndef _SpaceShip_H
#define _SpaceShip_H

#include "GameObject.h"
#include "Sprite.h"
#include "Vector2d.h"

class Renderer;
class Packet;

class SpaceShip : public GameObject {
public:
    explicit SpaceShip(unsigned int playerId, unsigned int objectId, const Renderer& renderer);

    virtual ~SpaceShip() = default;

    void update(float elapsed);

    void draw(Renderer& renderer);

    void write(Packet* packet);

    void read(Packet* packet);

    unsigned int getPlayerId() const;

    unsigned int getObjectId() const;

protected:
    void rotate(float angle);

    void thrust(bool onOff);

    virtual void beforeUpdate(float) {}

private:
    const unsigned int playerId_;
    const unsigned int objectId_;

    Sprite sprite_;

    Vector2d position_;
    Vector2d velocity_;
    Vector2d lookat_;
    float acceleration_;
};

#endif  // _SpaceShip_H
