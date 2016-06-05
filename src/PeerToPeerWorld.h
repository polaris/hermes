#ifndef _PeerToPeerWorld_H
#define _PeerToPeerWorld_H

#include "GameObject.h"

#include <boost/asio.hpp>

#include <unordered_map>
#include <functional>

using ObjectIdToGameObjectMap = std::unordered_map<uint32_t, GameObjectPtr>;
using ObjectIdToEndpointMap = std::unordered_map<uint32_t, boost::asio::ip::udp::endpoint>;
using ConfirmCollisionFunc = std::function<bool(uint32_t, const GameObject*, uint32_t, const GameObject*)>;

class PeerToPeerWorld {
public:
    PeerToPeerWorld(unsigned int width, unsigned int heigth, ConfirmCollisionFunc confirmCollisionFunc);

    void addLocalGameObject(uint32_t objectId, GameObjectPtr& gameObject);

    void removeLocalGameObject(uint32_t objectId);

    void addRemoteGameObject(uint32_t objectId, GameObjectPtr& gameObject, const boost::asio::ip::udp::endpoint& endpoint);

    void removeRemoteGameObject(uint32_t objectId);

    GameObject* getRemoteGameObject(uint32_t objectId);

    uint32_t getLocalGameObjectCount() const;

    void update(float elapsed);

    void draw(Renderer& renderer);

    void forEachLocalGameObject(std::function<void (uint32_t, GameObject*)> fun);

    void forEachRemoteGameObject(std::function<void (uint32_t, GameObject*)> fun);

    void forEachRemoteGameObject(const boost::asio::ip::udp::endpoint& endpoint, std::function<void (uint32_t, GameObject*)> fun);

    void removeLocalGameObjectIf(std::function<bool (uint32_t, GameObject*)> predicate);

private:
    const unsigned int width_;

    const unsigned int height_;

    ConfirmCollisionFunc confirmCollisionFunc_;

    ObjectIdToGameObjectMap localGameObjects_;

    ObjectIdToGameObjectMap remoteGameObjects_;

    ObjectIdToEndpointMap remoteEndpoints_;
};

#endif  // _PeerToPeerWorld_H
