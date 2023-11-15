#pragma once

#include "Entity.hpp"
#include "renderer/Renderer2D.hpp"
#include "camera/OrthographicCamera.hpp"

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
    OrthographicCamera m_camera;

public:

    Scene2D() : 
        m_renderer(Renderer2D()), 
        m_camera(OrthographicCamera(-1.0f, 1.0f, -1.0f, 1.0f))
    {}

    void init() {
        m_renderer.init();
    }

    // Entity Constructors:

    Entity create_circle() {
        return create_circle(glm::vec3(0), 0.1f);
    }

    Entity create_circle(glm::vec3 pos, float r, glm::vec4 color = glm::vec4(0.8, 0.3, 0, 1)) {
        return create_circle("Circle Entity", pos, r, color);
    };

    Entity create_circle(std::string name, glm::vec3 pos, float r, glm::vec4 color = glm::vec4(0.8, 0.3, 0, 1)) {
        Entity entity = create_entity(name);

        entity.add<Component::Circle>(color); 
        entity.add<Component::Transform>(pos, glm::vec3(r, r, 1));

        return entity;
    };

    Entity create_quad() {
        return create_quad(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 1.0f));
    }

    Entity create_quad(glm::vec3 position, glm::vec2 size, glm::vec4 color = glm::vec4(0.4, 0.7, 0, 1)) {
        return create_quad("Quad Entity", position, size, color);
    }

    Entity create_quad(std::string name, glm::vec3 position, glm::vec2 size, glm::vec4 color = glm::vec4(0.4, 0.7, 0, 1)) {
        Entity entity = create_entity(name);
    
        entity.add<Component::Quad>(color); 
        entity.add<Component::Transform>(position, glm::vec3(size, 1)); 

        return entity;
    }


    void render(glm::vec2 resolution) {
        // Fixing shorter dimension here to avoid edges +-1 being outside a standard window
        float aspect = resolution.x / resolution.y;
        m_camera.set_bounds(-aspect, aspect, -1.0f, 1.0f);
        m_camera.recalculate_view();

        m_renderer.begin(m_camera.m_projectionview);

        {
            auto view = m_registry.view<Component::Transform, Component::Quad>();
            for (auto entity : view) {
                auto [transform, quad] = view.get<Component::Transform, Component::Quad>(entity);
                m_renderer.draw_quad(transform.position, transform.scale, quad.color);
            }
        }

        {
            auto view = m_registry.view<Component::Transform, Component::Circle>();
            for (auto entity : view) {
                auto [transform, circle] = view.get<Component::Transform, Component::Circle>(entity);
                // TODO: should use scale directly
                // ... or transform matrix
                m_renderer.draw_circle(transform.position, transform.scale.x, circle.color); 
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