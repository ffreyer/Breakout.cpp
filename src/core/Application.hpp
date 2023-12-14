#pragma once

#include <iostream>

#include <glm/glm.hpp>

#include "opengl/Window.hpp"
#include "Events.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

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
    bool is_key_pressed(KeyCode key) { return m_window->is_key_pressed(key); }
    bool is_mouse_button_pressed(MouseCode button) { return m_window->is_mouse_button_pressed(button); }

// private:
    // process_events();
};