#include <iostream>

#include <glm/glm.hpp>

#include "Scene/Scene2D.hpp"
#include "physics/BoundingBox2D.hpp"
#include "core/Application.hpp"
#include "core/print.hpp"
#include "core/logging.hpp"

class MyApp : public Application {
private:
    Scene2D m_scene;
    Entity m_ball;
    Entity m_paddle;
    uint16_t m_score = 0;
    bool m_paused = false;

public:
    MyApp() {
        if (init("Test Window", 800, 600)) {
            m_scene.init();
            reset();
        }
    }

    void update(float delta_time) override {
        if (m_paused)
            return;

        // Physics update (TODO: cleanup)
        {
            entt::registry& reg = m_scene.get_registry();

            Component::Transform& c_transform = m_ball.get<Component::Transform>();
            Component::Velocity& c_vel = m_ball.get<Component::Velocity>();

            // TODO: fix
            glm::vec2 radius = glm::vec2(c_transform.scale);
            glm::vec2 delta = delta_time * c_vel.velocity;
            glm::vec3 new_pos = c_transform.position + glm::vec3(delta, 0.0f);

            auto colliders = reg.view<Component::Transform, Component::BoundingBox2D>();

            for (const auto collider : colliders) {
                if (collider == m_ball.get_entity())
                    continue;

                auto [transform, bbox] = colliders.get<Component::Transform, Component::BoundingBox2D>(collider);
                bbox.set(transform.position, transform.scale); // TODO: static version?
                HitResult result = bbox.collision_parameter(c_transform.position, radius, delta);

                if (result.hit && (0.0 <= result.parameter) && (result.parameter <= 1.0)) {
                    if (collider == m_paddle.get_entity()) {
                        // Ok the original game looks like it tries to do correct reflections
                        // but fails near the edge?
                    } else if (reg.all_of<Component::Destructable>(collider)) {
                        m_score++;
                        reg.destroy(collider);
                        std::cout << "Destroyed Block. Score: " << m_score << std::endl;
                    }
                    
                    glm::vec2 new_v = result.reflection_matrix * c_vel.velocity;
                    new_pos = c_transform.position + glm::vec3(result.parameter * delta + delta_time * (1 - result.parameter) * new_v, 0.0f);
                    c_vel.velocity = new_v;
                    break;
                }
            }   
            // std::cout << std::endl;
            c_transform.position = new_pos;
        }

        // Check gameover
        {
            Component::Transform c_transform = m_ball.get<Component::Transform>();
            if (c_transform.position.y < -1.0f) {
                std::cout << "Gameover" << std::endl;
                m_paused = true;
            }
        }

        // Render
        m_scene.render(m_window->get_window_size());
    }

    void on_event(AbstractEvent& event) override {
        dispatch<MouseMoveEvent>(BIND_EVENT_FN(update_paddle_position), event);
        dispatch<WindowResizeEvent>(BIND_EVENT_FN(update_paddle_position), event);

        // Update Paddle position
        if (event.type == EventType::KeyReleased) {
            KeyEvent& ke = static_cast<KeyEvent&>(event);
            if (ke.button == Key::R)
                reset();
            m_paused = false;

            return;
        }
    }

    void reset() {
        m_score = 0;
        m_scene.clear();

        // Add Ball
        m_ball = m_scene.create_circle(glm::vec3(0), 0.02f);
        m_ball.add<Component::Velocity>(glm::vec2(0.1f, 1.0f));

        // Add Bricks
        int columns = 10, rows = 6;
        float aspect = 600.0f / 800.0f;
        glm::vec2 brick_scale = glm::vec2(0.19, 0.04);

        for (int i = 0; i < columns; i++) {
            float x = (brick_scale.x + 0.01f) * i - 0.995f;
            for (int j = 0; j < rows; j++) {
                // aspect rescaling only works on init
                float y =  1.0f - (brick_scale.y + 0.01f / aspect) * (j + 1.0f); 
                Entity e = m_scene.create_quad(glm::vec3(x, y, 0.0f), brick_scale);
                e.add<Component::Destructable>();
            }
        }
        // {
        //     int i = 5, j = 6;
        //     float x = (brick_scale.x + 0.01f) * i - 0.995f;
        //     float y =  1.0f - (brick_scale.y + 0.01f / aspect) * (j + 1.0f); 
        //     Entity e = m_renderer.create_quad(glm::vec3(x, y, 0.0f), brick_scale);
        //     e.add<Component::Destructable>();
        // }

        // Add paddle
        m_paddle = m_scene.create_quad(glm::vec3(0.0f, -0.96f, 0.0f), brick_scale);

        // Add wall colliders
        // TODO: static colliders
        Entity wall_l = m_scene.create_entity("Wall left");
        wall_l.add<Component::BoundingBox2D>();
        wall_l.add<Component::Transform>(glm::vec3(-2.0f, -2.0f, 0.0f), glm::vec3(1.0f, 4.0f, 1.0f));
        wall_l.add<Component::Quad>(glm::vec3(0, 0, 0));
        
        Entity wall_r = m_scene.create_entity("Wall right");
        wall_r.add<Component::BoundingBox2D>();
        wall_r.add<Component::Transform>(glm::vec3(1.0f, -2.0f, 0.0f), glm::vec3(1.0f, 4.0f, 1.0f));
        wall_r.add<Component::Quad>(glm::vec3(0, 0, 0));

        Entity wall_top = m_scene.create_entity("Wall top");
        wall_top.add<Component::BoundingBox2D>();
        wall_top.add<Component::Transform>(glm::vec3(-2.0f, 1.0f, 0.0f), glm::vec3(4.0f, 1.0f, 1.0f));
        wall_top.add<Component::Quad>(glm::vec3(0, 0, 0));
    }


private:
    void update_paddle_position(MouseMoveEvent& e) { return update_paddle_position(); }
    void update_paddle_position(WindowResizeEvent& e) { return update_paddle_position(); }
    void update_paddle_position() {
        float x, w;
        x = get_mouse_position().x;
        w = (float) get_window_size().x;

        Component::BoundingBox2D& bbox = m_paddle.get<Component::BoundingBox2D>();
        Component::Transform& transform = m_paddle.get<Component::Transform>();
        transform.position.x = glm::clamp(x / w, 0.05f, 0.95f) * 2.0f - 1.0f - 0.5f * transform.scale.x;
        bbox.translate_lb_to(glm::vec2(transform.position));
    }
};



int main() {
    MyApp app;
    app.run();

    return 0;
}