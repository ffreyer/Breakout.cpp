#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "opengl/Window.hpp"
#include "Events.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// For now this is the object we implement an application in and Application
// acts as a manager
class Application;

class SubApp {
protected:
    Window* m_window = nullptr;
    bool m_running = false;
    std::string m_name;

public:
    SubApp(Window* window) : m_window(window) {};
    ~SubApp() = default;

    virtual void update(float delta_time) = 0;
    virtual void on_event(AbstractEvent& e) {};

    Window* get_window() const { return m_window; };

protected:
    glm::vec2 get_mouse_position()   { return m_window->get_mouse_position(); }
    glm::ivec2 get_window_position() { return m_window->get_window_position(); }
    glm::ivec2 get_window_size()     { return m_window->get_window_size(); }
    bool is_key_pressed(KeyCode key) { return m_window->is_key_pressed(key); }
    bool is_mouse_button_pressed(MouseCode button) { return m_window->is_mouse_button_pressed(button); }

    friend class Application;
};

class Application {
private:
    Window* m_window = nullptr;
    bool m_running = false;
    uint32_t m_current_app = 0;
    std::vector<std::unique_ptr<SubApp>> m_apps;

public:
    Application();
    ~Application();

    bool init(const char* name, int width, int height);
    void run();
    
    template <typename T, typename... Args>
    void push_back(Args&&... args) {
        SubApp* app = new T(m_window, std::forward<Args>(args)...);
        m_apps.emplace_back(app);
    }

    Window* get_window() const;

    void on_event(AbstractEvent& e);

    glm::vec2 get_mouse_position()   { return m_window->get_mouse_position(); }
    glm::ivec2 get_window_position() { return m_window->get_window_position(); }
    glm::ivec2 get_window_size()     { return m_window->get_window_size(); }
    bool is_key_pressed(KeyCode key) { return m_window->is_key_pressed(key); }
    bool is_mouse_button_pressed(MouseCode button) { return m_window->is_mouse_button_pressed(button); }
};