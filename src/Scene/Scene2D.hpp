#pragma once

#include "Entity.hpp"
#include "physics/BoundingBox2D.hpp"
#include "../renderer/Renderer2D.hpp"

// TODO: 
// Move definitions to cpp file

class AbstractScene {
protected:
    entt::registry m_registry;

public:
    // registry has a default constructor so this should be fine
    AbstractScene() = default;
    // registry is stack allocated so this should be fine
    ~AbstractScene() = default;

    Entity create_entity() {
        return create_entity("N/A");
    }

    Entity create_entity(std::string name) {
        // Entity entity = { m_registry, m_registry.create() };
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



class Scene2D : public AbstractScene {
private:
    Renderer2D m_renderer;

public:

    void init() {
        m_renderer.init();
    }

    // Entity Constructors:

    Entity create_circle() {
        Entity entity = create_entity("Circle Entity");

        entity.add<Component::Circle>(10.0f); 
        entity.add<Component::Position>(glm::vec3(0.0f, 0.0f, 0.0f));
        entity.add<Component::BoundingBox2D>(-10.0f, -10.0f, 10.0f, 10.0f);
        // entity.add<Component::CameraData>();

        return entity;
    };

    Entity create_quad() {
        return create_quad(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 1.0f));
    }

    Entity create_quad(glm::vec3 position, glm::vec2 size) {
        Entity entity = create_entity("Quad Entity");
    
        entity.add<Component::Quad>(size); 
        entity.add<Component::Position>(position); 
        entity.add<Component::BoundingBox2D>(glm::vec2(position), size);

        return entity;
    }


    void render() {
        m_renderer.begin();

        {
            auto view = m_registry.view<Component::Position, Component::Quad>();
            for (auto entity : view) {
                auto [position, quad] = view.get<Component::Position, Component::Quad>(entity);
                m_renderer.draw_quad(position, quad);
            }
        }

        {
            auto view = m_registry.view<Component::Position, Component::Circle>();
            for (auto entity : view) {
                auto [position, circle] = view.get<Component::Position, Component::Circle>(entity);
                m_renderer.draw_circle(position, circle);
            }
        }

        m_renderer.end();
    }

};


void debug_log(entt::registry& reg, entt::entity entity) {
    std::cout << "Deleting entity";
    std::string name = reg.get<Component::Name>(entity).name;
    // uint64_t id = reg.get<Component::ID>(entity).id;
    std::cout << " " << name << " " << (uint32_t) entity << std::endl;
}