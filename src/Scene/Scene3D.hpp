#pragma once

#include <vector>
#include <array>

#include "AbstractScene.hpp"
#include "callbacks.hpp"
#include "renderer/Renderer2D.hpp"
#include "camera/FirstPersonCamera.hpp"
#include "../core/print.hpp"
#include "../core/Events.hpp"
#include "../opengl/GLTexture.hpp"
#include "../opengl/Window.hpp"


// TODO: 
// Move definitions to cpp file

namespace Component {
    struct SimpleMesh {
        std::vector<uint32_t> indices;
        std::vector<float> vertices;
        GLVertexArray va;

        // TODO: check if move is the correct tool here
        SimpleMesh(std::vector<uint32_t>& idxs, std::vector<float>& data, GLBufferLayout& layout)
            : indices(idxs), vertices(data)
        {
            std::shared_ptr<GLIndexBuffer> ibuffer = std::make_shared<GLIndexBuffer>(indices.data(), sizeof(float) * indices.size());
            std::shared_ptr<GLVertexBuffer> vbuffer = std::make_shared<GLVertexBuffer>(vertices.data(), sizeof(float) * vertices.size());
            vbuffer->set_layout(layout);
            va.set(ibuffer);
            va.push(vbuffer);
        }

        void bind() const { va.bind(); }
        void unbind() const { va.unbind(); }
        uint32_t size() const { return va.index_count(); }
    };

    struct SimpleTexture2D {
        int width = 0, height = 0;
        GLTexture texture;

        SimpleTexture2D(const char* filepath) 
            : texture(GLTexture(2))
        {
            int channels;
            stbi_set_flip_vertically_on_load(true);
            unsigned char* image = stbi_load(filepath, &width, &height, &channels, 0);
            unsigned int format = channels_to_gl_type(channels);
            texture.set_internal_format(format);
            texture.set_data(image, format, width, height);
            stbi_image_free(image);
        };

        void bind(GLShader& shader, unsigned int slot) {
            char name[10];
            sprintf_s(name, 10, "texture%u", slot);
            shader.set_uniform(name, (int) slot);
            glActiveTexture(GL_TEXTURE0 + slot);
            texture.bind();
        }
        void bind() {
            texture.bind();
        };
        void unbind() {
            texture.unbind();
        };
    };
}

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
    std::shared_ptr<GLShader> m_mesh_shader;
    Window* m_window = nullptr;
    std::unique_ptr<SkyBox> skybox = nullptr;

public:

    Scene3D() : 
        m_camera(FirstPersonCamera())
    {}

    void init(Window* window) {
        m_window = window;

        // Example Cube
        std::vector<float> vertices = {
            // back
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,

            // front
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,

            // left
            -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,

            // right
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,

            // bottom
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f,

            // top
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        };

        std::vector<uint32_t> indices = {
            0, 1, 2, 1, 2, 3, // back
            4, 5, 6, 5, 6, 7, // front
            8, 9, 10, 9, 10, 11, // left
            12, 13, 14, 13, 14, 15, // right
            16, 17, 18, 17, 18, 19, // bottom
            20, 21, 22, 21, 22, 23  // top
        };

        Entity cube = create_entity("Simple Cube");
        auto layout = GLBufferLayout({
            GLBufferElement("Position", GLType::Float3),
            GLBufferElement("Normal", GLType::Float3),
            GLBufferElement("Texture Coordinates", GLType::Float2),
        });
        cube.add<Component::SimpleMesh>(indices, vertices, layout);
        cube.add<Component::Transform>();
        cube.add<Component::SimpleTexture2D>("../assets/wood_container.jpg");

        // shader
        m_mesh_shader = std::make_shared<GLShader>();
        m_mesh_shader->add_source("../assets/shaders/triangle.vert");
        m_mesh_shader->add_source("../assets/shaders/lighting.frag");
        m_mesh_shader->add_source("../assets/shaders/triangle.frag");
        m_mesh_shader->compile();

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

        auto view = m_registry.view<Component::SimpleMesh, Component::SimpleTexture2D, Component::Transform>();

        m_mesh_shader->bind();

        // camera
        m_mesh_shader->set_uniform("projectionview", m_camera.m_projectionview);
        // m_mesh_shader->set_uniform("projection", m_camera.m_projection);
        // m_mesh_shader->set_uniform("view", m_camera.m_view);

        // Basic directional light
        m_mesh_shader->set_uniform("light_direction", glm::normalize(glm::vec3(0.0f, -1.0f, 0.0)));
        m_mesh_shader->set_uniform("light_color", glm::vec3(0.8f, 0.95f, 1.0f));
        m_mesh_shader->set_uniform("ambient_color", glm::vec3(0.2f));

        for (entt::entity e : view) {
            auto& mesh = m_registry.get<Component::SimpleMesh>(e);
            auto& texture = m_registry.get<Component::SimpleTexture2D>(e);
            auto& transform = m_registry.get<Component::Transform>(e);
            m_mesh_shader->set_uniform("model", transform.get_matrix());
            m_mesh_shader->set_uniform("normalmatrix", transform.get_normalmatrix());
            texture.bind(*m_mesh_shader, 0);
            mesh.bind();
            glDrawElements(GL_TRIANGLES, mesh.size(), GL_UNSIGNED_INT, 0);
        }
        GLVertexArray::unbind();

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