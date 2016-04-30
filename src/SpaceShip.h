#ifndef _SpaceShip_H
#define _SpaceShip_H

#include "GameObject.h"
#include "Sprite.h"
#include "Vector2d.h"

class Renderer;
class Packet;

class SpaceShip : public GameObject {
public:
    explicit SpaceShip(const Renderer& renderer);

    virtual ~SpaceShip() = default;

    virtual void update(float elapsed) override;

    void draw(Renderer& renderer) override;

    void write(Packet* packet) override;

    void read(Packet* packet) override;

    uint32_t getClassId() const override;

    void setPlayerId(uint32_t playerId);

    uint32_t getPlayerId() const;

    void rotate(float angle);

    void thrust(bool onOff);

    static GameObjectPtr createInstance(const Renderer& renderer);

    enum { ClassId = 1 };

private:
    uint32_t playerId_;

    Sprite sprite_;

    Vector2d position_;
    Vector2d velocity_;
    Vector2d lookat_;
    float acceleration_;
};

using SpaceShipPtr = std::shared_ptr<SpaceShip>;

#endif  // _SpaceShip_H
