#pragma once

#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include "Scene/Scene3D.hpp"
#include "core/Application.hpp"
#include "core/print.hpp"
#include "core/logging.hpp"

class Example3D : public Application {
private:
    Scene3D m_scene;

public:
    Example3D() {
        if (init("Test Window", 800, 600)) {
            m_scene.init(get_window());
        }
    }

    void update(float delta_time) override {
        m_scene.update(delta_time);
        m_scene.render();
    };

    void on_event(AbstractEvent& event) override {
        m_scene.on_event(event);
    };

};