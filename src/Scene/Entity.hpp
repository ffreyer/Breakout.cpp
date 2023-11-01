#pragma once

#include <entt/entt.hpp>
#include <iostream>

#include "Components.hpp"
#include "../core/logging.hpp"

class Entity {
private:
    entt::registry* m_registry;
    entt::entity m_entity;

public:
    Entity() = default;

    Entity(entt::registry& registry, entt::entity entity) : 
        m_registry(&registry), m_entity(entity)
    {
        // INFO << "Created entity " << (uint32_t) m_entity << std::endl;
    }
    Entity(const Entity& other) = default;

    ~Entity() {
        // INFO << "Deleted entity " <<  m_entity << std::endl;
        // TODO: this clashes with Renderer destructor
        // m_registry->destroy(m_entity);
    }

    template <typename Component, typename... Args>
    void add(Args&&... args) const {
        m_registry->emplace<Component>(m_entity, std::forward<Args>(args)...);
    }

    template <typename Component, typename... Args>
    void set(Args&&... args) const {
        // patch with lambda
        // m_registry->patch<Component>(m_entity, [](auto& obj) { obj.field = x, obj.field = y })

        // replace the whole objects
        m_registry->replace<Component>(m_entity, std::forward<Args>(args)...);
    }

    template <typename Component>
    void remove() const {
        m_registry->erase<Component>(m_entity);
        // or remove to be save
        // m_registry->remove<Component>(m_entity);
    }

    template <typename Component>
    Component& get() const {
        return m_registry->get<Component>(m_entity);
    }

    template <typename... Component>
    bool has() const {
        return m_registry->all_of<Component...>(m_entity);
    }

    entt::entity get_entity() const {
        return m_entity;
    }
};
