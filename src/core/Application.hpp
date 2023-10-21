#pragma once

#include <iostream>

#include <glfw3.h>

#include "../opengl/Window.hpp"

class Application {
// private:
public:
    Window* m_window = nullptr;
    bool m_running = false;
    // directory maybe
    // events

public:
    Application() = default;
    ~Application() {
        delete m_window;
    }

    bool init(const char* name, int width, int height) {
        m_window = new Window(name, width, height);
        if (m_window->init())
            return true;
        
        // Failed
        std::cout << "Failed to initialize window!" << std::endl;
        delete m_window;
        m_window = nullptr;
        return false;
    };

    void run() {
        if (!m_window) {
            std::cout << "Window needs to be initialized first!" << std::endl;
            return;
        }

        double last_time = glfwGetTime();
        float delta_time;

        m_running = true;
        while (m_running) {
            // TODO:
            // handle events
            m_window->poll_events();

            delta_time = (float)(glfwGetTime() - last_time);
            last_time = glfwGetTime();

            update(delta_time);

            m_window->swap_buffers();

            if (m_window->should_close())
                m_running = false;
        }
    };

    virtual void update(float delta_time) = 0;
    // virtual bool on_event();

// private:
    // process_events();
};