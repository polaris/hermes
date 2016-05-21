#ifndef _PeerToPeerWorld_H
#define _PeerToPeerWorld_H

#include "GameObject.h"

#include <unordered_map>
#include <functional>

using ObjectIdToGameObjectMap = std::unordered_map<uint32_t, GameObjectPtr>;

class PeerToPeerWorld {
public:
    PeerToPeerWorld();

    void addLocalGameObject(uint32_t objectId, GameObjectPtr& gameObject);

    void removeLocalGameObject(uint32_t objectId);

    void addRemoteGameObject(uint32_t objectId, GameObjectPtr& gameObject);

    void removeRemoteGameObject(uint32_t objectId);

    GameObject* getRemoteGameObject(uint32_t objectId);

    uint32_t getLocalGameObjectCount() const;

    void update(float elapsed);

    void draw(Renderer& renderer);

    void forEachLocalGameObject(std::function<void (uint32_t, GameObject*)> fun);

private:
    ObjectIdToGameObjectMap localGameObjects_;
    ObjectIdToGameObjectMap remoteGameObjects_;
};

#endif  // _PeerToPeerWorld_H
