#ifndef _GameObjectRegistry_H
#define _GameObjectRegistry_H

#include "GameObject.h"

#include <cassert>
#include <unordered_map>
#include <functional>

class Renderer;

using CreateInstanceFunction = std::function<GameObjectPtr (const Renderer&)>;

class GameObjectRegistry {
public:
    template<typename T>
    void registerFunction() {
        assert(createInstanceFunctionMap_.find(T::ClassId) == createInstanceFunctionMap_.end());
        createInstanceFunctionMap_[T::ClassId] = T::createInstance;
    }

    GameObjectPtr createGameObject(uint32_t classId, const Renderer& renderer) const {
        auto itr = createInstanceFunctionMap_.find(classId);
        if (itr != createInstanceFunctionMap_.end()) {
            return (*itr).second(renderer);
        }
        return nullptr;
    }

    static GameObjectRegistry& get() {
        static GameObjectRegistry instance;
        return instance;
    }

private:
    GameObjectRegistry() {}
    std::unordered_map<uint32_t, CreateInstanceFunction> createInstanceFunctionMap_;
};

void registerGameObjects();

#endif  // _GameObjectRegistry_H
