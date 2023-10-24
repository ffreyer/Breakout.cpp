#include <iostream>

#include <glm/glm.hpp>

#include "renderer/renderer/Renderer2D.hpp"
#include "renderer/physics.hpp"
#include "core/Application.hpp"
#include "core/print.hpp"

class MyApp : public Application {
private:
    Renderer2D m_renderer;
    Entity m_ball;
    Entity m_paddle;
    uint16_t m_score = 0;
    bool m_paused = false;

public:
    MyApp() {
        if (init("Test Window", 800, 600)) {
            m_renderer.init();
            reset();
        }
    }

    void update(float delta_time) override {
        if (m_paused)
            return;

        // Physics update (TODO: cleanup)
        {
            Component::Name& mover_name = m_ball.get<Component::Name>();
            Component::ID& mover_id     = m_ball.get<Component::ID>();
            Component::Position& c_pos  = m_ball.get<Component::Position>();
            Component::Velocity& c_vel  = m_ball.get<Component::Velocity>();
            Component::Circle& c_circle = m_ball.get<Component::Circle>();

            glm::vec2 radius = 2.0f * c_circle.radius / glm::vec2(m_window->get_window_size());
            glm::vec2 delta = delta_time * c_vel.velocity;
            glm::vec3 new_pos = c_pos.position + glm::vec3(delta.x, delta.y, 0.0f);

            auto colliders = m_renderer.m_registry.view<Component::BoundingBox2D>();

            for (const auto collider : colliders) {
                if (collider == m_ball.get_entity())
                    continue;

                Component::BoundingBox2D bbox = m_renderer.m_registry.get<Component::BoundingBox2D>(collider);
                HitResult result = bbox.collision_parameter(c_pos.position, radius, delta);

                if (result.hit && (0.0 <= result.parameter) && (result.parameter <= 1.0)) {
                    Component::Name& collider_name = m_renderer.m_registry.get<Component::Name>(collider);
                    Component::ID& collider_id = m_renderer.m_registry.get<Component::ID>(collider);
                    // std::cout << mover_name.name << " " << mover_id.id << " collided with " << 
                        // collider_name.name << " " << collider_id.id << "  |  " << result.parameter <<
                        // "(" << result.normal.x << ", " << result.normal.y << ")" << std::endl;
                    
                    glm::vec2 new_v = c_vel.velocity + 2.0f * result.normal * abs(c_vel.velocity);
                    
                    // Paddle momentum transfer
                    if (collider == m_paddle.get_entity())
                        new_v = new_v + m_renderer.m_registry.get<Component::Velocity>(collider).velocity;
                    else if (m_renderer.m_registry.all_of<Component::Destructable>(collider)) {
                        m_score++;
                        m_renderer.m_registry.destroy(collider);
                        std::cout << "Destroyed Block. Score: " << m_score << std::endl;
                        // new_v = 1.1f * new_v; // creates bugs and also too much
                    }
                    
                    new_pos = c_pos.position + glm::vec3(result.parameter * delta + delta_time * (1 - result.parameter) * new_v, 0.0f);
                    c_vel.velocity = new_v;
                    break;
                }
            }   
            // std::cout << std::endl;
            c_pos.position = new_pos;
        }

        // Check gameover
        {
            Component::Position c_pos = m_ball.get<Component::Position>();
            if (c_pos.position.y < -1.0f) {
                std::cout << "Gameover" << std::endl;
                m_paused = true;
            }
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
        else if (event.type == EventType::KeyReleased) {
            KeyEvent& ke = static_cast<KeyEvent&>(event);
            if (ke.button == Key::R)
                reset();
            m_paused = false;

            return;
        }
        else
            return;

        Component::BoundingBox2D& bbox = m_paddle.get<Component::BoundingBox2D>();
        Component::Quad& quad = m_paddle.get<Component::Quad>();
        Component::Position& pos = m_paddle.get<Component::Position>();
        Component::Velocity& vel = m_paddle.get<Component::Velocity>();
        float new_x = glm::clamp(x / w, 0.05f, 0.95f) * 2.0f - 1.0f - 0.5f * quad.size.x;
        // smooth out velocity over multiple frames
        // weights: ... 0.125 0.25 0.5
        vel.velocity.x = 0.5 * (vel.velocity.x + (new_x - pos.position.x)); 
        pos.position.x = new_x;
        bbox.translate_lb_to(glm::vec2(pos.position));
    }

    void reset() {
        m_score = 0;
        m_renderer.reset();

        // Add Ball
        m_ball = m_renderer.create_circle();
        m_ball.add<Component::Velocity>(glm::vec2(0.1f, 0.5f));

        // Add Bricks
        int columns = 10, rows = 6;
        float aspect = 600.0f / 800.0f;
        glm::vec2 brick_scale = glm::vec2(0.19, 0.04);

        for (int i = 0; i < columns; i++) {
            float x = (brick_scale.x + 0.01f) * i - 0.995f;
            for (int j = 0; j < rows; j++) {
                // aspect rescaling only works on init
                float y =  1.0f - (brick_scale.y + 0.01f / aspect) * (j + 1.0f); 
                Entity e = m_renderer.create_quad(glm::vec3(x, y, 0.0f), brick_scale);
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
        m_paddle = m_renderer.create_quad(glm::vec3(0.0f, -0.96f, 0.0f), brick_scale);
        m_paddle.add<Component::Velocity>(glm::vec2(0.0f, 0.0f));

        // Add wall colliders
        Entity wall_l = m_renderer.create_entity("Wall left");
        wall_l.add<Component::BoundingBox2D>(-2.0f, -1.0f, -2.0f, 2.0f);
        Entity wall_r = m_renderer.create_entity("Wall right");
        wall_r.add<Component::BoundingBox2D>( 1.0f,  2.0f, -2.0f, 2.0f);
        m_renderer.create_entity("Wall top").add<Component::BoundingBox2D>(-2.0f, 2.0f, 1.0f, 2.0f);
    }
};


int main() {
    MyApp app;
    app.run();

    return 0;
}