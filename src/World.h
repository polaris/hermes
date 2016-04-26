#ifndef _World_H
#define _World_H

#include "GameObject.h"

#include <unordered_map>

using ObjectIdToGameObjectMap = std::unordered_map<unsigned int, GameObjectPtr>;

class World {
public:
    World();

    void add(unsigned int objectId, GameObjectPtr& gameObject);

    void remove(unsigned int objectId);

    void update(float elapsed);

    void draw(Renderer& renderer);

    void write(Packet* packet);

    void read(Packet* packet);

private:
    ObjectIdToGameObjectMap gameObjects_;
};

#endif  // _World_H
