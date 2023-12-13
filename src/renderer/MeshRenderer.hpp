#pragma once

#include <vector>

#include "../opengl/GLTexture.hpp"
#include "../opengl/GLTexture.hpp"
#include "../opengl/GLVertexArray.hpp"
#include "../opengl/GLShader.hpp"

#include "../Scene/Entity.hpp"

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

    // add cube components to existing entity
    void add_cube_mesh(Entity& cube) const;

    // rendering
    void begin(glm::mat4& projectionview) const;
    void draw_mesh(Entity e) const;
    void end() const;

    void begin_shadow(glm::mat4& projectionview) const;
    void draw_shadow_mesh(Entity e) const;
};