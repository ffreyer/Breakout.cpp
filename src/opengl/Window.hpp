#pragma once

#include <glad/gl.h>
#include <glfw3.h>

#include <string>
#include <glm/glm.hpp>

class Application;

class Window {
private:
    std::string m_name;
    int m_width, m_height;
    // For events
    Application* m_app = nullptr;
    glm::vec2 m_mouse_position  = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_window_position = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_window_size     = glm::vec2(0.0f, 0.0f);

public:
    GLFWwindow* m_window;

    Window(std::string name, int width, int height)
        : m_name(name), m_width(width), m_height(height), m_window(nullptr)
    {
    };

    ~Window() {
        // TODO: glfwDestroyWindow?
        if (m_window)
            glfwTerminate();
    };

    // doers
    bool init();
    void activate();
    void poll_events();
    void swap_buffers();

    // getters
    bool should_close();
    glm::vec2 get_sizef();

    // setters
    void set_vsync(bool active);
    void set_size(int width, int height);

    // implemented in Application.cpp
    bool connect_events(Application* app);
};