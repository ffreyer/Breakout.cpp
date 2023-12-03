#pragma once

#include "Entity.hpp"

class AbstractScene {
protected:
    entt::registry m_registry;

public:
    // registry has a default constructor so this should be fine
    AbstractScene() = default;
    // registry is stack allocated so this should be fine
    ~AbstractScene() = default;

    Entity create_entity(std::string name = "N/A") {
        Entity e = Entity(m_registry, m_registry.create());
        e.add<Component::Name>(name);
        return e;
    }

    entt::registry& get_registry() {
        return m_registry;
    }

    virtual void clear() {
        m_registry.clear();
    }
};