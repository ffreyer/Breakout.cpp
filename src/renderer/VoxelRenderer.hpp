#pragma once

#include <array>

#include <glm/glm.hpp>

#include "opengl/GLShader.hpp"
#include "opengl/GLVertexArray.hpp"
#include "opengl/GLTexture.hpp"

#include "Scene/Entity.hpp"

namespace Component {
    struct Chunk {
        static const size_t LENGTH = 16;
        static constexpr size_t SIZE = LENGTH * LENGTH * LENGTH;
        std::array<uint8_t, SIZE> data;

        Chunk(std::array<uint8_t, SIZE> x) 
            : data(x)
        {}

        static std::array<uint8_t, SIZE> sample_data() {
            std::array<uint8_t, SIZE> data;
            for (int z = 0; z < LENGTH; z++) {
                for (int y = 0; y < LENGTH; y++) {
                    for (int x = 0; x < LENGTH; x++) {
                        float cx = abs(x-7.5), cy = abs(y-7.5), cz = abs(z-7.5);
                        data[LENGTH * (LENGTH * z + y) + x] = 
                            (cx * cy > 3) * (cx * cz > 3) * (cy * cz > 3) * (y < 9);
                    }
                }
            }
            return data;
        }
    };
}

// TODO: Voxel probably a bit simpler on a base level...
class VoxelRenderer {
private:
    struct {
        std::shared_ptr<GLVertexArray> va = nullptr;
        std::shared_ptr<GLTexture> texture = nullptr;
        std::shared_ptr<GLShader> shader = nullptr;
    } render_data;

public:
    VoxelRenderer() {}
    ~VoxelRenderer() {}

    void init() {
        // This is a basic, simple implementation
        // We consider a 16^3 chunk of voxels.
        // Voxel information (i.e. voxel block type) is placed in a 
        // 3D texture with air being 0. This should allow us to discard
        // hidden faces in a geometry shader.
        // The Vertexarray contains static position related to the values
        // in the texture. They get translated by a Transform as a chunk.

        // Vertices
        glm::ivec3 vertices[Component::Chunk::SIZE];
        for (int z = 0; z < Component::Chunk::LENGTH; z++) {
            for (int y = 0; y < Component::Chunk::LENGTH; y++) {
                for (int x = 0; x < Component::Chunk::LENGTH; x++) {
                    vertices[x + Component::Chunk::LENGTH * (y + Component::Chunk::LENGTH * z)] = 
                        glm::ivec3(x, y, z);
                }
            }
        }

        GLBufferLayout layout = GLBufferLayout({ GLBufferElement("Position", GLType::Int3) });
        std::shared_ptr<GLVertexBuffer> vbuffer = std::make_shared<GLVertexBuffer>(
            vertices, sizeof(glm::ivec3) * Component::Chunk::SIZE, GLVertexBuffer::STREAM_DRAW);
        vbuffer->set_layout(layout);
        render_data.va = std::make_shared<GLVertexArray>();
        render_data.va->push(vbuffer);

        // Texture
        render_data.texture = std::make_shared<GLTexture>(3);
        render_data.texture->set_min_filter(GLTexture::NEAREST);
        render_data.texture->set_mag_filter(GLTexture::NEAREST);
        render_data.texture->set_internal_format(GLTexture::R8UI);

        // Shader
        render_data.shader = std::make_shared<GLShader>();
        render_data.shader->add_source("../assets/shaders/voxel.vert");
        render_data.shader->add_source("../assets/shaders/voxel.geom");
        render_data.shader->add_source("../assets/shaders/lighting.frag");
        render_data.shader->add_source("../assets/shaders/voxel.frag");
        render_data.shader->compile();
    }

    void begin(glm::mat4& projectionview) {
        render_data.shader->bind();
        render_data.va->bind();
        render_data.texture->bind();

        render_data.shader->set_uniform("projectionview", projectionview);
        // TODO:
        render_data.shader->set_uniform("light_direction", glm::normalize(glm::vec3(0.0f, -1.0f, 0.0)));
        render_data.shader->set_uniform("light_color", glm::vec3(0.8f, 0.95f, 1.0f));
        render_data.shader->set_uniform("ambient_color", glm::vec3(0.2f));
    }

    void render(Entity e) {
        auto& chunk = e.get<Component::Chunk>();
        auto& transform = e.get<Component::Transform>();
        render_data.texture->set_data(chunk.data, GLTexture::RED_INTEGER, Component::Chunk::LENGTH, Component::Chunk::LENGTH, Component::Chunk::LENGTH);
        render_data.shader->set_uniform("model", transform.get_matrix());
        render_data.shader->set_uniform("normalmatrix", transform.get_normalmatrix());
        glDrawArrays(GL_POINTS, 0, Component::Chunk::SIZE);
    }

    void end() {
        render_data.va->unbind();
        render_data.texture->unbind();
    }
};