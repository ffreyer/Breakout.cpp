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
        
        // Sample Cube
        Entity cube = m_scene.create_entity("Sample Cube");
        MeshRenderer::add_cube_mesh(cube);
        cube.add<Component::Transform>();
        cube.get<Component::Transform>().scale_by(glm::vec3(0.5f));
        cube.add<Component::SimpleTexture2D>("../assets/wood_container.jpg");

        // Sample Voxel Chunk
        Entity chunk = m_scene.create_entity("Sample Chunk");
        chunk.add<Component::Chunk>(Component::Chunk::sample_data());
        chunk.add<Component::Transform>();
        auto& transform = chunk.get<Component::Transform>();
        transform.scale = glm::vec3(1.0f / (float) Component::Chunk::LENGTH);
        transform.position = glm::vec3(-8.0f, -24.0f, -8.0f);
    };

    void update(float delta_time) override {
        m_scene.update(delta_time);
        m_scene.render();
    };

    void on_event(AbstractEvent& event) override {
        m_scene.on_event(event);
    };
};