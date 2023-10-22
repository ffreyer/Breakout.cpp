#include <iostream>

#include <glm/glm.hpp>

#include "renderer/renderer/Renderer2D.hpp"
#include "core/Application.hpp"

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
        // Init frame
        m_renderer.begin();

        // Render
        m_renderer.render();
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

        Component::Position& pos = m_paddle->get<Component::Position>();
        pos.position.x = glm::clamp(x / w, 0.05f, 0.95f) * 2.0f - 1.0f;
    }
};


int main() {
    MyApp app;
    app.run();

    return 0;
}