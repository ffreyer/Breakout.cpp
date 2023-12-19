#pragma once

#include <entt/entt.hpp>
#include <iostream>

#include "Components.hpp"
#include "core/logging.hpp"

class Entity {
private:
    entt::registry* m_registry;
    entt::entity m_entity;

public:
    Entity() = default;

    Entity(entt::registry* registry, entt::entity entity)
        :  m_registry(registry), m_entity(entity)
    {}

    Entity(entt::registry& registry, entt::entity entity)
        : m_registry(&registry), m_entity(entity)
    {}

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

    template <typename... Component>
    decltype(auto) get() const {
        return m_registry->get<Component...>(m_entity);
    }

    template <typename... Component>
    bool has() const {
        return m_registry->all_of<Component...>(m_entity);
    }

    entt::entity get_entity() const {
        return m_entity;
    }

    void destroy() const {
        m_registry->destroy(m_entity);
    }

    void schedule_delete() const {
        if (!has<Component::ScheduledDelete>())
            m_registry->emplace<Component::ScheduledDelete>(m_entity);
    }

    friend inline bool operator==(const Entity& e1, const Entity& e2) {
        return (e1.m_registry == e2.m_registry) && (e1.m_entity == e2.m_entity);
    }
    friend inline bool operator!=(const Entity& e1, const Entity& e2) {
        return !(e1 == e2);
    }
    operator const entt::entity() { return m_entity; }
};
