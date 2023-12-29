#pragma once

#include <vector>

#include "opengl/GLTexture.hpp"
#include "opengl/GLTexture.hpp"
#include "opengl/GLVertexArray.hpp"
#include "opengl/GLShader.hpp"

#include "Scene/Entity.hpp"

namespace Component {
    struct SimpleMesh {
        std::vector<uint32_t> indices;
        std::vector<float> vertices;
        GLVertexArray va;

        // TODO: check if move is the correct tool here
        SimpleMesh(std::vector<uint32_t>& idxs, std::vector<float>& data, GLBufferLayout& layout);
    };

    struct SimpleTexture2D {
        int width = 0, height = 0;
        GLTexture texture;

        SimpleTexture2D(const char* filepath);
    };
}

// TODO: make a parent class for this
class MeshRenderer {
private:
    std::shared_ptr<GLShader> m_shader = nullptr;
    std::shared_ptr<GLShader> m_shadow_shader = nullptr;

public:
    MeshRenderer() = default;
    void init();
    GLShader& get_shader() { return *m_shader; }


    // rendering
    void begin(glm::mat4& projectionview) const;
    void draw_mesh(Entity e) const;
    void end() const;

    void begin_shadow(glm::mat4& projectionview) const;
    void draw_shadow_mesh(Entity e) const;

    // add cube components to existing entity
    static void add_cube_mesh(Entity& cube) {
        static std::vector<float> vertices = {
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
        static std::vector<uint32_t> indices = {
            0, 1, 2, 1, 2, 3, // back
            4, 5, 6, 5, 6, 7, // front
            8, 9, 10, 9, 10, 11, // left
            12, 13, 14, 13, 14, 15, // right
            16, 17, 18, 17, 18, 19, // bottom
            20, 21, 22, 21, 22, 23  // top
        };
        static GLBufferLayout layout = GLBufferLayout({
            GLBufferElement("Position", GLType::Float3),
            GLBufferElement("Normal", GLType::Float3),
            GLBufferElement("Texture Coordinates", GLType::Float2),
        });

        cube.add<Component::SimpleMesh>(indices, vertices, layout);
    }
};