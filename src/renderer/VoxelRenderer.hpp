#pragma once

#include <array>

#include <glm/glm.hpp>

#include "opengl/GLShader.hpp"
#include "opengl/GLVertexArray.hpp"
#include "opengl/GLTexture.hpp"

#include "Scene/Entity.hpp"
#include "TextureAtlas.hpp"

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
                           (cx * cy > 3) * (cx * cz > 3) * (cy * cz > 3) * (y < 9) * (x + 16 * z);
                    }
                }
            }
            return data;
        }
    };
}

class VoxelRenderer {
private:
    struct {
        std::shared_ptr<GLVertexArray> va = nullptr;
        std::shared_ptr<GLShader> shader = nullptr;
        std::shared_ptr<GLShader> shadow_shader = nullptr;
        std::shared_ptr<GLTexture> block_id = nullptr;
        std::shared_ptr<GLBuffer> uv_idx_map = nullptr; 
        std::shared_ptr<RegularTextureAtlas> texture_map = nullptr;
    } render_data;

public:
    VoxelRenderer() {}
    ~VoxelRenderer() {}

    void init();
    GLShader& get_shader() { return *render_data.shader; }

    void begin(glm::mat4& projectionview) const;
    void render(Entity e) const; 
    void end() const;

    void begin_shadow(glm::mat4& projectionview) const;
    void render_shadow(Entity e) const; 
};