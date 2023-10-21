#include <iostream>

#include <glad/gl.h>

#include "opengl/Window.hpp"
#include "renderer/renderer/Renderer2D.hpp"

int main() {
    int width = 800, height = 600;
    float aspect = (float) height / (float) width;
    Window window = Window("Test Window", width, height);
    if (!window.init())
        return -1;

    // Init Renderer
    Renderer2D renderer;

    // Add Ball
    Entity ball = renderer.create_circle();
    // ball.set<Component::Position>(glm::vec3(0.5f, 0.0f, 0.0f));

    // RenderObject::Circle ball = RenderObject::Circle();
    // ball.set_position(glm::vec3(0.5, 0, 0));
    // ball.set_velocity(glm::vec3(0, 0.5, 0));

    // Add Bricks
    int columns = 10, rows = 6;
    glm::vec2 brick_scale = glm::vec2(0.19, 0.04);

    for (int i = 0; i < columns; i++) {
        float x = (brick_scale.x + 0.01f) * (i + 0.5f) - 1.0f;
        for (int j = 0; j < rows; j++) {
            float y =  1.0f - (brick_scale.y + 0.01f / aspect) * (j + 0.5f); // aspect rescaling only works on init
            renderer.create_quad(glm::vec3(x, y, 0.0f), brick_scale);
        }
    }

    // Add paddle
    Entity paddle = renderer.create_quad(glm::vec3(0.0f, -0.96f, 0.0f), brick_scale);

    // depth test
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 


    // main loop
    double delta_time = 0.0, last_time = glfwGetTime();
    while (!window.should_close()) {
        // Input handling
        window.poll_events();
        delta_time = glfwGetTime() - last_time;
        last_time = glfwGetTime();

        // Init frame
        renderer.begin();

        // Update Paddle
        // TODO: consider scales and window size
        double mx, my;
        Component::Quad& quad = paddle.get<Component::Quad>();
        glfwGetCursorPos(window.m_window, &mx, &my);
        quad.position.x = glm::clamp((float) mx / (float) width, 0.05f, 0.95f) * 2.0f - 1.0f;

        // Render
        renderer.render();

        // Display rendered frame
        window.swap_buffers();
    }

    return 0;
}