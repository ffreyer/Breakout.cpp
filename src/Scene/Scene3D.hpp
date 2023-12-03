#pragma once

#include "AbstractScene.hpp"
#include "callbacks.hpp"
#include "renderer/Renderer2D.hpp"
#include "camera/FirstPersonCamera.hpp"
#include "../core/print.hpp"
#include "../core/Events.hpp"
#include "../opengl/GLTexture.hpp"

#include <vector>
#include <array>

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
        GLTexture<2> texture;

        SimpleTexture2D(const char* filepath) {
            int channels;
            stbi_set_flip_vertically_on_load(true);
            unsigned char* image = stbi_load(filepath, &width, &height, &channels, 0);
            unsigned int format;
            switch (channels) {
            case 1: format = GLTexture<2>::RED;  break;
            case 2: format = GLTexture<2>::RG;   break;
            case 3: format = GLTexture<2>::RGB;  break;
            case 4: format = GLTexture<2>::RGBA; break;
            }
            texture.set_internal_format(format);
            texture.set_data(image, format, width, height);
            stbi_image_free(image);
        };

        void bind(GLShader& shader, unsigned int slot) {
            char name[10];
            sprintf_s(name, 10, "texture%u", slot);
            shader.set_uniform(name, 0);
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
    FirstPersonCamera m_camera;
    std::shared_ptr<GLShader> m_mesh_shader;

public:

    Scene3D() : 
        m_camera(FirstPersonCamera())
    {}

    void init() {
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
    }

    void on_resize(WindowResizeEvent& e) {
        m_camera.m_aspect = (float) e.size.x / (float) e.size.y;
        m_camera.recalculate_projection();
    }

    void update(float delta_time) {
        resolve_on_update();
        resolve_scheduled_deletes();

        auto view = m_registry.view<Component::Transform>();
        for(entt::entity e : view) {
            auto& transform = m_registry.get<Component::Transform>(e);
            transform.rotate_by(glm::normalize(glm::vec3(-1.0f, 1.0f, -0.5f)), delta_time);
        }
    }

    void render() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        m_camera.recalculate_view();

        auto view = m_registry.view<Component::SimpleMesh, Component::SimpleTexture2D, Component::Transform>();
        // auto view = m_registry.view<Component::SimpleMesh, Component::Transform>();

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

};


// void debug_log(entt::registry& reg, entt::entity entity) {
//     std::cout << "Deleting entity";
//     std::string name = reg.get<Component::Name>(entity).name;
//     // uint64_t id = reg.get<Component::ID>(entity).id;
//     std::cout << " " << name << " " << (uint32_t) entity << std::endl;
// }