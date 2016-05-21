#ifndef _PeerToPeerWorld_H
#define _PeerToPeerWorld_H

#include "GameObject.h"

#include <unordered_map>
#include <functional>

using ObjectIdToGameObjectMap = std::unordered_map<uint32_t, GameObjectPtr>;

class PeerToPeerWorld {
public:
    PeerToPeerWorld();

    void add(uint32_t objectId, GameObjectPtr& gameObject);

    void remove(uint32_t objectId);

    GameObject* getGameObject(uint32_t objectId);

    uint32_t getGameObjectCount() const;

    void update(float elapsed);

    void draw(Renderer& renderer);

    void forEachGameObject(std::function<void (uint32_t, GameObject*)> fun);

private:
    ObjectIdToGameObjectMap gameObjects_;
};

#endif  // _PeerToPeerWorld_H
