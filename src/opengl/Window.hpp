#pragma once

#include <glad/gl.h>
#include <glfw3.h>

#include <string>
#include <glm/glm.hpp>

class Application;

class Window {
private:
    std::string m_name;
    // For events
    Application* m_app = nullptr;
    glm::vec2 m_mouse_position  = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_window_position = glm::ivec2(0, 0);
    glm::vec2 m_window_size     = glm::ivec2(0, 0);

public:
    GLFWwindow* m_window;

    Window(std::string name, int width, int height)
        : m_name(name), m_window_size(width, height), m_window(nullptr)
    {
    };

    ~Window() {
        // TODO: glfwDestroyWindow?
        if (m_window)
            glfwTerminate();
    };

    // doers
    bool init();
    void activate() const;
    void poll_events() const;
    void swap_buffers() const;

    // getters
    bool should_close() const;
    glm::vec2 get_mouse_position() const;
    glm::ivec2 get_window_position() const;
    glm::ivec2 get_window_size() const;

    // setters
    void set_vsync(bool active) const;
    void set_size(int width, int height);

    // implemented in Application.cpp
    bool connect_events(Application* app);
};