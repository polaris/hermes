#ifndef _World_H
#define _World_H

#include "GameObject.h"

#include <unordered_map>
#include <functional>

using ObjectIdToGameObjectMap = std::unordered_map<unsigned int, GameObjectPtr>;

class World {
public:
    World();

    void add(unsigned int objectId, GameObjectPtr& gameObject);

    void remove(unsigned int objectId);

    GameObject* getGameObject(unsigned int objectId);

    std::size_t getGameObjectCount() const;

    void update(float elapsed);

    void draw(Renderer& renderer);

    void forEachGameObject(std::function<void (unsigned int, GameObject*)> fun);

private:
    ObjectIdToGameObjectMap gameObjects_;
};

#endif  // _World_H
