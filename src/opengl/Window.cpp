#include "Window.hpp"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

bool Window::init() {
    // Init GLFW
    if (!glfwInit()) {
        std::cout << "Failed to Init GLFW" << std::endl;
        return false;
    }
    
    // Init OpenGL Version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create Window
    m_window = glfwCreateWindow((int) m_window_size.x, (int) m_window_size.y, m_name.c_str(), NULL, NULL);
    if (m_window == nullptr) {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return false;
    }
    
    activate();

    // vsync on by default
    glfwSwapInterval(1);

    // Init OpenGL interface
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    // set initial viewport
    glViewport(0, 0, (int) m_window_size.x, (int) m_window_size.y);
    // update on resize
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    return true;
}

void Window::activate() const {
    glfwMakeContextCurrent(m_window);
}

void Window::poll_events() const {
    glfwPollEvents();
}

void Window::swap_buffers() const {
    glfwSwapBuffers(m_window);
}

bool Window::should_close() const {
    return glfwWindowShouldClose(m_window);
}

glm::vec2 Window::get_mouse_position() const {
    double mx, my;
    glfwGetCursorPos(m_window, &mx, &my);
    return glm::vec2(mx, my);
    // Or just this?
    // return m_mouse_position;
}
glm::ivec2 Window::get_window_position() const {
    return m_window_position;
}
glm::ivec2 Window::get_window_size() const {
    return m_window_size;
}

bool Window::is_key_pressed(KeyCode key) const {
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}
bool Window::is_mouse_button_pressed(MouseCode button) const {
    return glfwGetMouseButton(m_window, button) == GLFW_PRESS;
}

void Window::set_vsync(bool active) const {
    activate(); // for savety
    glfwSwapInterval(active ? 1 : 0);
}

void Window::set_size(int width, int height) {
    activate(); // for savety
    glfwSetWindowSize(m_window, width, height);
}


// TODO:
// create better event system



