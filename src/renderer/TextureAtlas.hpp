#pragma once

#include <unordered_map>

#include <glm/glm.hpp>

#include "opengl/GLTexture.hpp"

class AbstractTextureAtlas : public GLTexture {
protected:
    glm::ivec2 m_size = glm::ivec2(0);

public:
    AbstractTextureAtlas() 
        : GLTexture(2)
    {}

    void load(std::string&& filepath) {
        load(filepath);
    }
    void load(std::string& filepath) {
        int channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(filepath.c_str(), &(m_size.x), &(m_size.y), &channels, 0);
        set_data(data, channels_to_gl_type(channels), m_size.x, m_size.y);
        stbi_image_free(data);
    }
};


class RegularTextureAtlas : public AbstractTextureAtlas {
private:
    glm::vec2 m_uv_size = glm::vec2(0);
    glm::ivec2 m_px_size = glm::ivec2(0);
public:
    RegularTextureAtlas() : AbstractTextureAtlas() {}

    void set_element_pixel_size(size_t x , size_t y) {
        m_px_size = glm::ivec2(x, y);
        m_uv_size = glm::vec2(m_px_size) / glm::vec2(m_size);
    }

    void set_element_uv_size(float x , float y) {
        m_uv_size = glm::vec2(x, y);
        m_px_size = glm::ivec2(m_uv_size * glm::vec2(m_size));
    }

    // indices -> uv
    glm::vec4 get_uv(size_t x, size_t y) {
        glm::vec4 uv;
        uv.x = x * m_uv_size.x;
        uv.y = y * m_uv_size.y;
        uv.z = (x + 1.0f) * m_uv_size.x;
        uv.w = (y + 1.0f) * m_uv_size.y;
        return uv;
    }

    glm::vec2 get_uv_size() const { return m_uv_size; }
    glm::ivec2 get_px_size() const { return m_px_size; }
    glm::ivec2 get_index_size() const { return m_size / m_px_size; }

};