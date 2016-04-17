#ifndef _World_H
#define _World_H

#include "GameObject.h"

#include <vector>

class World : public GameObject {
public:
    World();

    void add(GameObjectPtr& gameObject);
    void add(GameObjectPtr&& gameObject);

    void remove(GameObjectPtr& gameObject);

    void update(float elapsed) override;

    void draw(Renderer& renderer) override;

private:
    std::vector<GameObjectPtr> gameObjects;
};

#endif  // _World_H
