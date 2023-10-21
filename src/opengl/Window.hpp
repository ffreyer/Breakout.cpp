#pragma once

#include <glad/gl.h>
#include <glfw3.h>

#include <string>
#include <glm/glm.hpp>

class Window {
private:
    std::string m_name;
    int m_width, m_height;

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

};