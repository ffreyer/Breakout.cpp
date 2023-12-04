#pragma once

#include <vector>
#include <array>

#include "AbstractScene.hpp"
#include "callbacks.hpp"
#include "renderer/MeshRenderer.hpp"
#include "camera/FirstPersonCamera.hpp"
#include "../core/print.hpp"
#include "../core/Events.hpp"
#include "../opengl/Window.hpp"


#include "../opengl/GLTexture.hpp"

// TODO: 
// Move definitions to cpp file


class Scene3D : public AbstractScene {
private:
    struct SkyBox {
        GLVertexArray va;
        GLShader shader;
        GLCubeMap cubemap;

        SkyBox(std::array<std::string, 6> filepaths) {
            // load texture
            int width, height, channels;
            unsigned char* data;
            cubemap.bind();
            cubemap.set_internal_format(GLCubeMap::RGB);
            stbi_set_flip_vertically_on_load(false);
            for (int i = 0; i < 6; i++) {
                data = stbi_load(filepaths[i].c_str(), &width, &height, &channels, 0);
                cubemap.set_data(data, channels_to_gl_type(channels), width, height, GLCubeMap::RIGHT + i);
                stbi_image_free(data);
            }

            // generate vertexarray
            float vertices[] = {
                -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                -1.0f,  1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f,  1.0f
            };
            auto layout = GLBufferLayout({GLBufferElement("Position", GLType::Float3)});
            std::shared_ptr<GLVertexBuffer> buffer = std::make_shared<GLVertexBuffer>(vertices, sizeof(vertices));
            buffer->set_layout(layout);
            va.push(buffer);

            // Generate shader
            shader.add_source("../assets/shaders/skybox.vert");
            shader.add_source("../assets/shaders/skybox.frag");
            shader.compile();
        }

        void render(FirstPersonCamera camera){
            glDepthFunc(GL_LEQUAL); 
            shader.bind();
            va.bind();
            cubemap.bind();
            shader.set_uniform("view", glm::mat4(glm::mat3(camera.m_view)));
            shader.set_uniform("projection", camera.m_projection);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthFunc(GL_LESS); 
        }
    };

    FirstPersonCamera m_camera;
    Window* m_window = nullptr;
    std::unique_ptr<SkyBox> skybox = nullptr;
    MeshRenderer m_mesh_renderer;

public:

    Scene3D() : 
        m_camera(FirstPersonCamera())
    {}

    void init(Window* window) {
        m_window = window;
        m_mesh_renderer.init();

        Entity cube = create_entity("Simple Cube");
        m_mesh_renderer.add_cube_mesh(cube);
        cube.add<Component::Transform>();
        cube.add<Component::SimpleTexture2D>("../assets/wood_container.jpg");

        // cam
        m_camera.eyeposition(glm::vec3(3.0f, 0.0, 0.0f));
        m_camera.lookat(glm::vec3(0.0f));

        // Skybox
        skybox = std::make_unique<SkyBox>(std::array<std::string, 6>({
            "../assets/skybox/right.jpg",
            "../assets/skybox/left.jpg",
            "../assets/skybox/top.jpg",
            "../assets/skybox/bottom.jpg",
            "../assets/skybox/front.jpg",
            "../assets/skybox/back.jpg",
        }));
    }

    void on_event(AbstractEvent& event) {
        dispatch<WindowResizeEvent>(BIND_EVENT_FN(on_resize), event);
        dispatch<MouseMoveEvent>(BIND_EVENT_FN(on_mouse_move), event);
    }

    void update(float delta_time) {
        resolve_on_update();
        resolve_scheduled_deletes();

        auto view = m_registry.view<Component::Transform>();
        for(entt::entity e : view) {
            auto& transform = m_registry.get<Component::Transform>(e);
            transform.rotate_by(glm::normalize(glm::vec3(-1.0f, 1.0f, -0.5f)), delta_time);
        }

        // camera motion (keyboard)
        float step = 3.0f * delta_time;
        if (m_window->is_key_pressed(Key::W))  m_camera.dolly( step);
        if (m_window->is_key_pressed(Key::S))  m_camera.dolly(-step);
        if (m_window->is_key_pressed(Key::A))  m_camera.truck(-step);
        if (m_window->is_key_pressed(Key::D))  m_camera.truck( step);
        if (m_window->is_key_pressed(Key::Space))  m_camera.pedestal( step);
        if (m_window->is_key_pressed(Key::LeftControl))  m_camera.pedestal(-step);
        if (m_window->is_key_pressed(Key::Q))  m_camera.roll( step);
        if (m_window->is_key_pressed(Key::E))  m_camera.roll(-step);
    }

    void render() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        m_camera.recalculate_view();

        // Render meshes
        auto view = m_registry.view<Component::SimpleMesh, Component::SimpleTexture2D, Component::Transform>();
        m_mesh_renderer.begin(m_camera.m_projectionview);
        for (entt::entity e : view)
            m_mesh_renderer.draw_mesh(Entity(m_registry, e));
        m_mesh_renderer.end();

        skybox->render(m_camera);
    }

private:
    // Systems
    void resolve_on_update() {
        auto view = m_registry.view<Component::OnUpdate>();
        for (entt::entity e : view) {
            Entity ent = Entity(m_registry, e);
            m_registry.get<Component::OnUpdate>(e).callback(ent);
        }
    }

    void resolve_scheduled_deletes() {
        auto view = m_registry.view<Component::ScheduledDelete>();
        m_registry.destroy(view.begin(), view.end());
    }

    void on_resize(WindowResizeEvent& e) {
        m_camera.m_aspect = (float) e.size.x / (float) e.size.y;
        m_camera.recalculate_projection();
    }

    void on_mouse_move(MouseMoveEvent& e) {
        if (m_window->is_mouse_button_pressed(Mouse::ButtonLeft)) {
            glm::vec2 delta = 0.003f * e.delta;
            m_camera.pan(delta.x);
            m_camera.tilt(delta.y);
        }
    }
};