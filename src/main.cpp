#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include "Scene/Scene2D.hpp"
#include "physics/Physics2D.hpp"
#include "core/Application.hpp"
#include "core/print.hpp"
#include "core/logging.hpp"

class MyApp : public Application {
private:
    Scene2D m_scene;
    std::vector<Entity> m_balls;
    Entity m_paddle;
    uint16_t m_score = 0;
    bool m_paused = false;
    PhysicsEngine2D m_physics;

public:
    MyApp() {
        if (init("Test Window", 800, 600)) {
            m_scene.init();
            m_physics.init(m_scene.get_registry());
            reset();
        }
    }

    void update(float delta_time) override {
        if (m_paused)
            return;

        // Simulate physics world
        m_physics.process(delta_time);
        m_scene.update(delta_time);

        // Check gameover
        {
            // Delete out of bounds balls
            for (auto it = this->m_balls.begin(); it != this->m_balls.end(); it++) {
                auto ball = *it;
                auto transform = ball.get<Component::Transform>();
                if (transform.position.y < -1.0f) {
                    m_balls.erase(it);
                    break;
                }
            }

            // If no balls exist we dead
            if (m_balls.empty()) {
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

    void create_ball(glm::vec2 pos = glm::vec2(0), glm::vec2 vel = glm::vec2(0.1f, 1.0f)) {
        Entity ball = m_scene.create_circle("Ball", glm::vec3(pos, 0), 0.02f);
        ball.add<Component::Collision2D>(glm::vec2(0.0f), 1.0f, 1.0f);
        ball.add<Component::Motion2D>(vel);
        m_balls.push_back(ball);
    }

    void screen_shake() { m_scene.screen_shake(); }

    void reset() {
        m_score = 0;
        m_scene.clear();
        m_physics.clear();

        // Add Ball
        create_ball();

        // Add Bricks
        int columns = 10, rows = 6;
        float aspect = 600.0f / 800.0f;
        glm::vec2 brick_scale = glm::vec2(0.19, 0.04);

        Callback::Function on_brick_collision = [this](Entity& e, Entity& other){
            Callback::destroy(e, other);
            this->screen_shake();
        };

        for (int i = 0; i < columns; i++) {
            float x = (brick_scale.x + 0.01f) * i - 0.995f;
            for (int j = 0; j < rows; j++) {
                // aspect rescaling only works on init
                float y =  1.0f - (brick_scale.y + 0.01f / aspect) * (j + 1.0f); 
                char name[16];
                sprintf_s(name, 16, "Brick[%i, %i]", i, j);
                Entity e = m_scene.create_quad(name, glm::vec3(x, y, 0.0f), brick_scale);
                e.add<Component::Collision2D>(Component::Collision2D::Rect2D, on_brick_collision);
            }
        }

        // Add paddle
        m_paddle = m_scene.create_quad("Paddle", glm::vec3(0.0f, -0.96f, 0.0f), brick_scale);
        m_paddle.add<Component::Collision2D>(Component::Collision2D::Rect2D);

        // Add wall colliders
        Entity wall_l = m_scene.create_entity("Wall left");
        wall_l.add<Component::Collision2D>(Component::Collision2D::Rect2D);
        wall_l.add<Component::Transform>(glm::vec3(-2.0f, -2.0f, 0.0f), glm::vec3(1.0f, 4.0f, 1.0f));
        wall_l.add<Component::Quad>(glm::vec3(0, 0, 0));
        
        Entity wall_r = m_scene.create_entity("Wall right");
        wall_r.add<Component::Collision2D>(Component::Collision2D::Rect2D);
        wall_r.add<Component::Transform>(glm::vec3(1.0f, -2.0f, 0.0f), glm::vec3(1.0f, 4.0f, 1.0f));
        wall_r.add<Component::Quad>(glm::vec3(0, 0, 0));

        Entity wall_top = m_scene.create_entity("Wall top");
        wall_top.add<Component::Collision2D>(Component::Collision2D::Rect2D);
        wall_top.add<Component::Transform>(glm::vec3(-2.0f, 1.0f, 0.0f), glm::vec3(4.0f, 1.0f, 1.0f));
        wall_top.add<Component::Quad>(glm::vec3(0, 0, 0));

        m_physics.construct();
    }

private:
    void update_paddle_position(MouseMoveEvent& e) { return update_paddle_position(); }
    void update_paddle_position(WindowResizeEvent& e) { return update_paddle_position(); }
    void update_paddle_position() {
        Component::Transform& transform = m_paddle.get<Component::Transform>();

        float x = get_mouse_position().x;
        float w = (float) get_window_size().x;
        float aspect = (float) w / get_window_size().y;
        float sx = 0.5f * transform.scale.x;

        float cam_x = aspect * (x / w  * 2.0f - 1.0f);
        transform.position.x = glm::clamp(cam_x, sx - 1.0f, 1.0f - sx) - sx;

        // Need to manually trigger updates for objects we move ourself :/
        m_physics.update_entity(m_paddle.get_entity());
    }
};



int main() {
    MyApp app;
    app.run();

    return 0;
}