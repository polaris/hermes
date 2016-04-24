#ifndef _World_H
#define _World_H

#include "GameObject.h"

#include <unordered_map>

using ObjectIdToGameObjectMap = std::unordered_map<unsigned int, GameObjectPtr>;

class World : public GameObject {
public:
    World();

    void add(unsigned int objectId, GameObjectPtr& gameObject);

    void remove(unsigned int objectId);

    void update(float elapsed) override;

    void draw(Renderer& renderer) override;

    void write(Packet* packet) override;

    void read(Packet* packet) override;

private:
    ObjectIdToGameObjectMap gameObjects_;
};

#endif  // _World_H
