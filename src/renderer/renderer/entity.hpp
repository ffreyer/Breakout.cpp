#pragma once

#include <entt/entt.hpp>
#include <iostream>

namespace Component {
    static uint64_t s_id = 0;

    struct ID{
        uint64_t id;
        ID() {
            id = s_id;
            s_id = s_id + 1;
        }
    };

}

class Entity {
private:
    entt::registry* m_registry;
    entt::entity m_entity;

public:
    Entity() = default;

    Entity(entt::registry& registry, entt::entity entity) : 
        m_registry(&registry), m_entity(entity)
    {
        add<Component::ID>();
        // std::cout << "Entity " << get<Component::ID>().id << " created" << std::endl;
    }
    Entity(const Entity& other) = default;

    ~Entity() {
        // std::cout << "Entity " << get<Component::ID>().id << " destroyed" << std::endl;
        // TODO: this clashes with Renderer destructor
        // m_registry->destroy(m_entity);
    }

    template <typename Component, typename... Args>
    void add(Args&&... args) {
        m_registry->emplace<Component>(m_entity, std::forward<Args>(args)...);
    }

    template <typename Component, typename... Args>
    void set(Args&&... args) {
        // patch with lambda
        // m_registry->patch<Component>(m_entity, [](auto& obj) { obj.field = x, obj.field = y })

        // replace the whole objects
        m_registry->replace<Component>(m_entity, std::forward<Args>(args)...);
    }

    template <typename Component>
    void remove() {
        m_registry->erase<Component>(m_entity);
        // or remove to be save
        // m_registry->remove<Component>(m_entity);
    }

    template <typename Component>
    Component& get() {
        return m_registry->get<Component>(m_entity);
    }

    template <typename... Component>
    bool has() {
        return m_registry->all_of<Component...>(m_entity);
    }

    entt::entity get_entity() {
        return m_entity;
    }
};
