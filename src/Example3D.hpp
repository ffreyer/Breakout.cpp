#pragma once

#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include "Scene/Scene3D.hpp"
#include "core/Application.hpp"
#include "core/logging.hpp"

class Example3D : public SubApp {
private:
    Scene3D m_scene;

public:
    Example3D(Window* window) : SubApp(window) {
        m_scene.init(get_window());
        m_name = "3D Example";
    };

    void update(float delta_time) override {
        m_scene.update(delta_time);
        m_scene.render();
    };

    void on_event(AbstractEvent& event) override {
        m_scene.on_event(event);
    };
};