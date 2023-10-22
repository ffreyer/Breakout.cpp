#include <iostream>

#include <glm/glm.hpp>

#include "renderer/renderer/Renderer2D.hpp"
#include "renderer/physics.hpp"
#include "core/Application.hpp"
#include "core/print.hpp"

class MyApp : public Application {
private:
    Renderer2D m_renderer;
    Entity* m_ball = nullptr;
    Entity* m_paddle = nullptr;

public:
    MyApp() {
        if (init("Test Window", 800, 600)) {
            m_renderer.init();

            // Add Ball
            m_ball = &m_renderer.create_circle();
            m_ball->add<Component::Velocity>(glm::vec2(0.0f, 0.5f));

            // Add Bricks
            int columns = 10, rows = 6;
            float aspect = 600.0f / 800.0f;
            glm::vec2 brick_scale = glm::vec2(0.19, 0.04);

            for (int i = 0; i < columns; i++) {
                float x = (brick_scale.x + 0.01f) * (i + 0.5f) - 1.0f;
                for (int j = 0; j < rows; j++) {
                    // aspect rescaling only works on init
                    float y =  1.0f - (brick_scale.y + 0.01f / aspect) * (j + 0.5f); 
                    m_renderer.create_quad(glm::vec3(x, y, 0.0f), brick_scale);
                }
            }

            // Add paddle
            m_paddle = &m_renderer.create_quad(glm::vec3(0.0f, -0.96f, 0.0f), brick_scale);
        }
    }

    void update(float delta_time) override {
        // Physics update (TODO: cleanup)
        auto movers = m_renderer.m_registry.view<Component::Position, Component::Velocity>();
        auto colliders = m_renderer.m_registry.view<Component::BoundingBox2D>();
        for (auto entity : movers) {
            Component::Position& c_pos = m_renderer.m_registry.get<Component::Position>(entity);
            Component::Velocity& c_vel = m_renderer.m_registry.get<Component::Velocity>(entity);
            glm::vec2 delta = delta_time * c_vel.velocity;
            glm::vec3 new_pos = c_pos.position + glm::vec3(delta.x, delta.y, 0.0f);

            for (auto collider : colliders) {
                if (collider == entity)
                    continue;
                
                Component::BoundingBox2D bbox = m_renderer.m_registry.get<Component::BoundingBox2D>(collider);
                HitResult result = bbox.collision_parameter(c_pos.position, delta);
                if (result.hit && (0.0 < result.parameter) && (result.parameter <= 1.0)) {
                    new_pos = c_pos.position + glm::vec3(result.parameter * delta - 2 * (1 - result.parameter) * result.normal * abs(delta), 0.0f);
                    c_vel.velocity = c_vel.velocity + 2.0f * result.normal * abs(c_vel.velocity);
                    break;
                }
            }   
            c_pos.position = new_pos;
        }

        // Render
        m_renderer.begin();
        m_renderer.render();
        m_renderer.end();
    }

    void on_event(AbstractEvent& event) override {

        // Update Paddle position
        float x, w;
        if (event.type == EventType::MouseMoved) {
            x = static_cast<MouseMoveEvent&>(event).position.x;
            w = (float) get_window_size().x;
        }
        else if (event.type == EventType::WindowResize) {
            x = get_mouse_position().x;
            w = (float) static_cast<WindowResizeEvent&>(event).size.x;
        }
        else
            return;

        Component::BoundingBox2D& bbox = m_paddle->get<Component::BoundingBox2D>();
        Component::Position& pos = m_paddle->get<Component::Position>();
        pos.position.x = glm::clamp(x / w, 0.05f, 0.95f) * 2.0f - 1.0f;
        bbox.translate_center_to(glm::vec2(pos.position));
    }
};


int main() {
    MyApp app;
    app.run();

    return 0;
}