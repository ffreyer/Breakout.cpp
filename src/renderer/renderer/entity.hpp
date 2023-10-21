#pragma once

#include <entt/entt.hpp>

class Entity {
private:
    entt::registry& m_registry;
    entt::entity m_entity;

public:
    Entity(entt::registry& registry) 
        : m_registry(registry)
    {
        m_entity = registry.create();
    }

    ~Entity() {
        // TODO: this clashes with Renderer destructor
        // m_registry.destroy(m_entity);
    }

    template <typename Component, typename... Args>
    void add(Args&&... args) {
        m_registry.emplace<Component>(m_entity, std::forward<Args>(args)...);
    }

    template <typename Component, typename... Args>
    void set(Args&&... args) {
        // patch with lambda
        // m_registry.patch<Component>(m_entity, [](auto& obj) { obj.field = x, obj.field = y })

        // replace the whole objects
        m_registry.replace<Component>(m_entity, std::forward<Args>(args)...);
    }

    template <typename Component>
    void remove() {
        m_registry.erase<Component>(m_entity);
        // or remove to be save
        // m_registry.remove<Component>(m_entity);
    }

    template <typename Component>
    Component& get() {
        return m_registry.get<Component>(m_entity);
    }

    template <typename... Component>
    bool has() {
        return m_registry.all_of<Component...>(m_entity);
    }
};
