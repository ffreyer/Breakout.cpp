#pragma once

#include <vector>
#include <array>

#include <glm/gtx/io.hpp>

#include "AbstractScene.hpp"
#include "callbacks.hpp"

#include "renderer/MeshRenderer.hpp"
#include "renderer/VoxelRenderer.hpp"
#include "renderer/VoxelRenderer2.hpp"
#include "renderer/Skybox.hpp"

#include "camera/FirstPersonCamera.hpp"
#include "camera/OrthographicCamera.hpp"
#include "core/Events.hpp"
#include "opengl/Window.hpp"

#include "opengl/GLTexture.hpp"
#include "opengl/GLFramebuffer.hpp"

// TODO: 
// Move definitions to cpp file

void maybe_print_error() {
    auto error = glGetError();
    if (error != GL_NO_ERROR)
        std::cout << "ERROR!" << std::endl;
}

class Scene3D : public AbstractScene {
private:
    FirstPersonCamera m_camera;
    Window* m_window = nullptr;

    std::unique_ptr<SkyBox> skybox = nullptr;
    MeshRenderer m_mesh_renderer;
    VoxelRenderer m_voxel_renderer;
    VoxelRenderer2 m_voxel_renderer2;

    // TODO: move into new structure/class
    std::unique_ptr<GLFramebuffer> m_framebuffer = nullptr;
    std::unique_ptr<GLShader> m_to_screen = nullptr;
    std::unique_ptr<GLVertexArray> m_screen_quad = nullptr;
    // Shadow pass
    OrthographicCamera m_shadow_camera;


public:

    Scene3D() : 
        m_camera(FirstPersonCamera()), m_shadow_camera(OrthographicCamera())
    {}

    void init(Window* window) {
        m_window = window;
        m_mesh_renderer.init();
        m_voxel_renderer.init();
        m_voxel_renderer2.init();

        // Sample Cube
        Entity cube = create_entity("Sample Cube");
        m_mesh_renderer.add_cube_mesh(cube);
        cube.add<Component::Transform>();
        cube.get<Component::Transform>().scale_by(glm::vec3(0.5f));
        cube.add<Component::SimpleTexture2D>("../assets/wood_container.jpg");

        // Sample Voxel Chunk
        Entity chunk = create_entity("Sample Chunk");
        chunk.add<Component::Chunk>(Component::Chunk::sample_data());
        chunk.add<Component::Transform>();
        auto& transform = chunk.get<Component::Transform>();
        transform.scale = glm::vec3(1.0f / (float) Component::Chunk::LENGTH);
        transform.position = glm::vec3(-8.0f, -24.0f, -8.0f);

        // cam
        m_camera.eyeposition(glm::vec3(50.0f, 10.0f, 50.0f));
        m_camera.lookat(glm::vec3(0.0f));
        m_camera.m_far = 1000.0f;
        m_camera.recalculate_projection();

        // Skybox
        skybox = std::make_unique<SkyBox>(std::array<std::string, 6>({
            "../assets/skybox/right.jpg",
            "../assets/skybox/left.jpg",
            "../assets/skybox/top.jpg",
            "../assets/skybox/bottom.jpg",
            "../assets/skybox/front.jpg",
            "../assets/skybox/back.jpg",
        }));

        // framebuffer test
        m_framebuffer = std::make_unique<GLFramebuffer>();
        m_framebuffer->attach_texture(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, GL_DEPTH_COMPONENT);
        AbstractGLTexture& shadowmap = m_framebuffer->get(0);
        shadowmap.set_min_filter(GLTexture::NEAREST);
        shadowmap.set_mag_filter(GLTexture::NEAREST);
        m_framebuffer->resize(1024, 1024);
        m_framebuffer->disable_color();
        m_framebuffer->unbind();

        // copy framebuffer to screen
        m_to_screen = std::make_unique<GLShader>();
        m_to_screen->add_source("../assets/shaders/full_screen.vert");
        m_to_screen->add_source("../assets/shaders/full_screen.frag");
        m_to_screen->compile();

        m_screen_quad = std::make_unique<GLVertexArray>();
        float vertices[] = {
            -1.0f, -1.0f,
            -1.0f,  1.0f,
             1.0f, -1.0f,
             1.0f,  1.0f,
        };
        auto layout = GLBufferLayout({GLBufferElement("Position", GLType::Float2)});
        std::shared_ptr<GLVertexBuffer> buffer = std::make_shared<GLVertexBuffer>(vertices, sizeof(vertices));
        buffer->set_layout(layout);
        m_screen_quad->push(buffer);

        // TODO: ortho camera (derive these from perspective camera...)
        m_shadow_camera.lrbt(-1.0f, 1.0f, -1.0f, 1.0f);
        m_shadow_camera.near(0.1f);
        m_shadow_camera.far(10.0f);
        m_shadow_camera.recalculate_projection();

        // Voxel world
        Entity world = create_entity("Sample World");
        world.add<Component::VoxelWorld>(glm::ivec3(1024, 128, 1024));
        // world.add<Component::VoxelWorld>(glm::ivec3(512, 100, 512));
        m_voxel_renderer2.update_world(world.get<Component::VoxelWorld>());
    }

    void on_event(AbstractEvent& event) {
        dispatch<WindowResizeEvent>(BIND_EVENT_FN(on_resize), event);
        dispatch<MouseMoveEvent>(BIND_EVENT_FN(on_mouse_move), event);
    }

    void update(float delta_time) {
        resolve_on_update();
        resolve_scheduled_deletes();

        auto view = m_registry.view<Component::Transform>(entt::exclude<Component::Chunk>);
        for(entt::entity e : view) {
            auto& transform = m_registry.get<Component::Transform>(e);
            transform.rotate_by(glm::normalize(glm::vec3(-1.0f, 1.0f, -0.5f)), delta_time);
        }

        // camera motion (keyboard)
        float step = 30.0f * delta_time;
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
        /*
        // TODO: lighting variables
        glm::vec3 light_direction = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0));
        m_camera.recalculate_view();

        // Shadow pass 
        m_framebuffer->bind();
        glViewport(0, 0, 1024, 1024);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);

        { // Setup matrix
            m_shadow_camera.viewdirection(light_direction);
            float view_distance = glm::length(m_camera.eyeposition());
            m_shadow_camera.eyeposition(-(1 + view_distance) * light_direction);
            m_shadow_camera.up(glm::vec3(1, 0, 0));
            m_shadow_camera.recalculate_view();
        }

        {
            auto view = m_registry.view<Component::SimpleMesh, Component::SimpleTexture2D, Component::Transform>();
            m_mesh_renderer.begin_shadow(m_shadow_camera.m_projectionview);
            for (entt::entity e : view)
                m_mesh_renderer.draw_shadow_mesh(Entity(m_registry, e));
            m_mesh_renderer.end();
        }

        {
            auto view = m_registry.view<Component::Chunk, Component::Transform>();
            m_voxel_renderer.begin_shadow(m_shadow_camera.m_projectionview);
            for (entt::entity e : view)
                m_voxel_renderer.render_shadow(Entity(m_registry, e));
            m_voxel_renderer.end();
        }

        m_framebuffer->unbind();

        // Main Pass
        glm::ivec2 window_size = m_window->get_window_size();
        glViewport(0, 0, window_size.x, window_size.y);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        // glEnable(GL_MULTISAMPLE);

        m_camera.recalculate_view();
        auto& tex = m_framebuffer->get(0);

        // Render meshes
        {
            auto view = m_registry.view<Component::SimpleMesh, Component::SimpleTexture2D, Component::Transform>();
            m_mesh_renderer.begin(m_camera.m_projectionview);
            GLShader& shader = m_mesh_renderer.get_shader();
            shader.set_uniform("shadowmap", tex);
            shader.set_uniform("lightspace", m_shadow_camera.m_projectionview);
            for (entt::entity e : view)
                m_mesh_renderer.draw_mesh(Entity(m_registry, e));
            m_mesh_renderer.end();
        }

        {
            auto view = m_registry.view<Component::Chunk, Component::Transform>();
            m_voxel_renderer.begin(m_camera.m_projectionview);
            GLShader& shader = m_voxel_renderer.get_shader();
            shader.set_uniform("shadowmap", tex);
            shader.set_uniform("lightspace", m_shadow_camera.m_projectionview);
            for (entt::entity e : view)
                m_voxel_renderer.render(Entity(m_registry, e));
            m_voxel_renderer.end();
        }

        */

        {
            m_camera.recalculate_view();

            glm::ivec2 window_size = m_window->get_window_size();
            glViewport(0, 0, window_size.x, window_size.y);
            glClearColor(0.2f, 0.3f, 0.3f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            auto view = m_registry.view<Component::VoxelWorld>();

            m_voxel_renderer2.begin(m_camera.m_projectionview, m_camera.eyeposition());
            for (entt::entity e : view)
                m_voxel_renderer2.render(Entity(m_registry, e));
            m_voxel_renderer2.end();
        }

        skybox->render(m_camera.m_view, m_camera.m_projection);

        // copy to screen
        // m_framebuffer->unbind();

        // glClearColor(0.1f, 0.8f, 0.1f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);
        // glDisable(GL_DEPTH_TEST);

        // {
        //     m_to_screen->bind();
        //     m_screen_quad->bind();
        //     auto& tex = m_framebuffer->get(0);
        //     tex.bind();
        //     m_to_screen->set_uniform("tex", 0);
        //     glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        // }
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