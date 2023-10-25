#pragma once

#include <iostream>

#include <glm/glm.hpp>

#include "../opengl/Window.hpp"
#include "Events.hpp"


class Application {
public:
    Window* m_window = nullptr;
    bool m_running = false;
    // directory maybe
    // events

public:
    Application();
    ~Application();

    Window* get_window() const;

    bool init(const char* name, int width, int height);
    void run();

    virtual void update(float delta_time) = 0;
    virtual void on_event(AbstractEvent& e) {};

    glm::vec2 get_mouse_position()   { return m_window->get_mouse_position(); }
    glm::ivec2 get_window_position() { return m_window->get_window_position(); }
    glm::ivec2 get_window_size()     { return m_window->get_window_size(); }

// private:
    // process_events();
};