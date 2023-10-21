#include <iostream>

#include <glm/glm.hpp>

#include "renderer/renderer/Renderer2D.hpp"
#include "core/Application.hpp"

class MyApp : public Application {
private:
    Renderer2D m_renderer;
    Entity* m_paddle = nullptr;

public:
    MyApp() {
        if (init("Test Window", 800, 600)) {
            m_renderer.init();

            // Add Ball
            Entity ball = m_renderer.create_circle();
            // ball.set<Component::Position>(glm::vec3(0.5f, 0.0f, 0.0f));

            // RenderObject::Circle ball = RenderObject::Circle();
            // ball.set_position(glm::vec3(0.5, 0, 0));
            // ball.set_velocity(glm::vec3(0, 0.5, 0));

            // Add Bricks
            int columns = 10, rows = 6;
            float aspect = 600.0f / 800.0f;
            glm::vec2 brick_scale = glm::vec2(0.19, 0.04);

            for (int i = 0; i < columns; i++) {
                float x = (brick_scale.x + 0.01f) * (i + 0.5f) - 1.0f;
                for (int j = 0; j < rows; j++) {
                    float y =  1.0f - (brick_scale.y + 0.01f / aspect) * (j + 0.5f); // aspect rescaling only works on init
                    m_renderer.create_quad(glm::vec3(x, y, 0.0f), brick_scale);
                }
            }

            // Add paddle
            m_paddle = &m_renderer.create_quad(glm::vec3(0.0f, -0.96f, 0.0f), brick_scale);
        }
    }

    void update(float delta_time) {
        // Init frame
        m_renderer.begin();

        // Update Paddle
        // TODO: consider scales and window size
        double mx, my;
        Component::Quad& quad = m_paddle->get<Component::Quad>();
        glfwGetCursorPos(m_window->m_window, &mx, &my);
        quad.position.x = glm::clamp((float) mx / 800.0f, 0.05f, 0.95f) * 2.0f - 1.0f;

        // Render
        m_renderer.render();
    }
};


int main() {
    MyApp app;
    app.run();

    return 0;
}