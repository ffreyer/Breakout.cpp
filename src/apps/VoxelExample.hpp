#pragma once

#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include "Scene/Scene3D.hpp"
#include "core/Application.hpp"
#include "core/logging.hpp"

class VoxelExample : public SubApp {
private:
    Scene3D m_scene;

public:
    VoxelExample(Window* window) : SubApp(window) {
        m_scene.init(get_window());
        m_name = "Voxel Example";

        // cam
        FirstPersonCamera& camera = m_scene.get_camera();
        camera.eyeposition(glm::vec3(50.0f, 10.0f, 50.0f));
        camera.lookat(glm::vec3(0.0f));
        camera.m_far = 1000.0f;
        camera.recalculate_projection();

        // Voxel world
        Entity world = m_scene.create_entity("Sample World");
        // world.add<Component::VoxelWorld>(glm::ivec3(1024, 128, 1024));
        world.add<Component::VoxelWorld>(glm::ivec3(512, 100, 512));
    };

    void update(float delta_time) override {
        m_scene.update(delta_time);
        m_scene.render();
    };

    void on_event(AbstractEvent& event) override {
        m_scene.on_event(event);
    };
};