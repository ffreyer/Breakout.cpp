#include "Window.hpp"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

bool Window::init()
{
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
    m_window = glfwCreateWindow(m_width, m_height, m_name.c_str(), NULL, NULL);
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
    glViewport(0, 0, m_width, m_height);
    // update on resize
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    return true;
}

void Window::activate()
{
    glfwMakeContextCurrent(m_window);
}

void Window::poll_events()
{
    glfwPollEvents();
}

void Window::swap_buffers()
{
    glfwSwapBuffers(m_window);
}

bool Window::should_close()
{
    return glfwWindowShouldClose(m_window);
}

glm::vec2 Window::get_sizef()
{
    return glm::vec2(m_width, m_height);
}

void Window::set_vsync(bool active)
{
    activate(); // for savety
    glfwSwapInterval(active ? 1 : 0);
}

void Window::set_size(int width, int height)
{
    activate(); // for savety
}


// TODO:
// create better event system



