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

    Window* get_window();

    bool init(const char* name, int width, int height);
    void run();

    virtual void update(float delta_time) = 0;
    virtual void on_event(AbstractEvent& e) { return; }

// private:
    // process_events();
};