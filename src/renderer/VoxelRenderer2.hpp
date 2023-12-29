/*
Following the ideas from https://www.youtube.com/watch?v=4xs66m1Of4A we can 
replace the per-voxel mesh generation with static global quads which stretch the
entire visible scene. In 2D:

  1  2  3  4  5
  +--+--+--+--+ 6
  |  |  |  |  |
  +--+--+--+--+ 7
  |  |  |  |  |
  +--+--+--+--+ 8
  |  |  |  |  | 
  +--+--+--+--+ 9

The example above would draw 3 * 4 voxels using 9 planes.

On scaling:
Let's consider a chunk of size C^3 and X * Y * Z chunks
- Dumb Meshing: (just drawing a full cube per voxel)
    X * Y * Z chunks * C^3 voxels per chunk * 6 quads
- solid face culling: (remove faces between solid blocks)
  number depends on geometry/layout of chunk. With a dense cube as the base, 
  removing lines from corner to corner will reduce the number of quads generated
  and creating caves (as simple as adding a 1-voxel hole on the surface) will 
  add quads.
  optimistic average case (full cube)
    X * Y * Z chunks * 6 sides * C^2 quads per side
  worst case: (checkerboard of voxels)
    X * Y * Z * C^3 / 2 voxels * 6 faces 
- greedy meshing:
  varies, best case (chunk is dense):
    X * Y * Z * 6 faces
  worst case - same as above:
    X * Y * Z * C^3 / 2 voxels * 6 faces 
  average case - definitely better than above, but unclear scaling
- global quads:
  does not scale linearly with the number of chunks!
    X * C + Y * C + Z * C quads
*/

#pragma once

#include <vector>
#include <cstdlib>

#include <glm/glm.hpp>

#include "opengl/GLShader.hpp"
#include "opengl/GLVertexArray.hpp"
#include "opengl/GLTexture.hpp"

#include "Scene/Entity.hpp"
#include "TextureAtlas.hpp"

namespace Component {
    // TODO: compression
    struct VoxelWorld {
        uint8_t* data = nullptr;
        glm::ivec3 size;
        bool needs_update = true;

        VoxelWorld(uint8_t* _data, glm::ivec3 _size) 
            : data(_data), size(_size)
        {}

        ~VoxelWorld() {
            delete data;
        }

        VoxelWorld(glm::ivec3 _size) : size(_size) {
            data = new uint8_t[size.x * size.y * size.z];
            std::cout << sizeof(float) * size.x * size.y * size.z << std::endl;
            
            for (size_t x = 0; x < size.x; x++) {
                float xsin = sin(0.01f * x);

                for (size_t y = 0; y < size.y; y++) {
                    float yn = (3.0f * y) / size.y - 1.5f;
                
                    for (size_t z = 0; z < size.z; z++) {
                        float zsin = sin(0.02f * z);

                        data[z + size.z * (y + size.y * x)] = 
                            (xsin * zsin > yn) * 
                            static_cast<uint8_t>(rand() % 0xff);
                    }
                }
            }

        }
    };
}

class VoxelRenderer2 {
private:
    struct {
        std::shared_ptr<GLVertexArray> va = nullptr;
        std::shared_ptr<GLShader> shader = nullptr;
        std::shared_ptr<GLTexture> block_id = nullptr;
    } render_data;

public:
    VoxelRenderer2() {}
    ~VoxelRenderer2() {}

    void init() {
        // quad vertex array
        float vertices[] = {
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
        };
        GLBufferLayout layout = GLBufferLayout({ GLBufferElement("Position", GLType::Float3) });
        std::shared_ptr<GLVertexBuffer> vbuffer = std::make_shared<GLVertexBuffer>(
            vertices, sizeof(vertices), GLVertexBuffer::DYNAMIC_DRAW
        );
        vbuffer->set_layout(layout);
        render_data.va = std::make_shared<GLVertexArray>();
        render_data.va->push(vbuffer);

        // Voxel data
        render_data.block_id = std::make_shared<GLTexture>(3);
        render_data.block_id->set_min_filter(GLTexture::NEAREST);
        render_data.block_id->set_mag_filter(GLTexture::NEAREST);
        render_data.block_id->set_internal_format(GLTexture::R8UI);

        // Shader
        render_data.shader = std::make_shared<GLShader>();
        render_data.shader->add_source("../assets/shaders/3D/voxel2.vert");
        render_data.shader->add_source("../assets/shaders/3D/voxel2.frag");
        render_data.shader->compile();
    }
    GLShader& get_shader() { return *render_data.shader; }

    void begin(glm::mat4& projectionview, glm::vec3 eyeposition) const {
       render_data.shader->bind();
       render_data.va->bind();

       render_data.shader->set_uniform("block_id", *render_data.block_id);
       render_data.shader->set_uniform("projectionview", projectionview);
       render_data.shader->set_uniform("eyeposition", eyeposition);

        // TODO:
        render_data.shader->set_uniform("light_direction", glm::normalize(glm::vec3(-0.2f, -1.0f, -0.5)));
        render_data.shader->set_uniform("light_color", glm::vec3(0.8f, 0.95f, 1.0f));
        render_data.shader->set_uniform("ambient_color", glm::vec3(0.2f));
    };

    void render(Entity e) const {
        auto& world = e.get<Component::VoxelWorld>();
        if (world.needs_update)
            update_world(world);

        for (int dim : {1, 0, 2}) {
            render_data.va->update(0, get_quad(world.size, dim), 4 * 3 * sizeof(float));
            render_data.shader->set_uniform("dim", dim);
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, world.size[dim] + 1);
        }
    }; 

    void end() const {
        render_data.va->unbind();
        render_data.block_id->unbind();
    };

    void update_world(Component::VoxelWorld& world) const {
        // TODO: lazy local updates from component?
        render_data.block_id->set_data(
            world.data, GLTexture::RED_INTEGER, 
            world.size.x, world.size.y, world.size.z
        );
        world.needs_update = false;
    }

    // void begin_shadow(glm::mat4& projectionview) const;
    // void render_shadow(Entity e) const; 

private:
    float* get_quad(glm::ivec3 size, int dim) const {
        static float quad_xy[] = {
            -0.5f * size.x, -0.5f * size.y, 0.0f,
            +0.5f * size.x, -0.5f * size.y, 0.0f,
            -0.5f * size.x, +0.5f * size.y, 0.0f,
            +0.5f * size.x, +0.5f * size.y, 0.0f,
        };
        static float quad_xz[] = {
            -0.5f * size.x, 0.0f, -0.5f * size.z,
            +0.5f * size.x, 0.0f, -0.5f * size.z,
            -0.5f * size.x, 0.0f, +0.5f * size.z,
            +0.5f * size.x, 0.0f, +0.5f * size.z,
        };
        static float quad_yz[] = {
            0.0f, -0.5f * size.y, -0.5f * size.z,
            0.0f, +0.5f * size.y, -0.5f * size.z,
            0.0f, -0.5f * size.y, +0.5f * size.z,
            0.0f, +0.5f * size.y, +0.5f * size.z,
        };

        switch (dim) {
        case 0:
            return quad_yz;
        case 1:
            return quad_xz;
        case 2:
            return quad_xy;
        default:
            std::cout << "Should not be reached." << std::endl; // TODO: exception
            return nullptr;
        }
    }
};