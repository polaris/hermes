#ifndef _World_H
#define _World_H

#include "GameObject.h"

#include <unordered_map>
#include <functional>

using ObjectIdToGameObjectMap = std::unordered_map<uint32_t, GameObjectPtr>;

class World {
public:
    World();

    virtual ~World() = default;

    void add(uint32_t objectId, GameObjectPtr& gameObject);

    void remove(uint32_t objectId);

    GameObject* getGameObject(uint32_t objectId);

    uint32_t getGameObjectCount() const;

    virtual void update(float elapsed);

    void draw(Renderer& renderer);

    void forEachGameObject(std::function<void (uint32_t, GameObject*)> fun);

    void removeIf(std::function<bool (uint32_t, GameObject*)> predicate);

protected:
    ObjectIdToGameObjectMap gameObjects_;
};

#endif  // _World_H
