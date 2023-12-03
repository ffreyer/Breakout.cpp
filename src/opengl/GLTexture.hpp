#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <stb_image.h>

#include "GLTypeConvert.hpp"

template <int Dim>
class GLTexture {
private:
    unsigned int m_id = 0;
    size_t m_size;
    bool m_mipmapped = false;
    unsigned int m_internal_format = GL_RGBA;

public:
    enum : unsigned int {
        // Wrapping directions
        WRAP_X = GL_TEXTURE_WRAP_S, WRAP_Y = GL_TEXTURE_WRAP_T, WRAP_Z = GL_TEXTURE_WRAP_R,
        // wrapping modes
        REPEAT = GL_REPEAT, MIRRORED_REPEAT = GL_MIRRORED_REPEAT, CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE, CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
        // filtering (no mipmap)
        NEAREST = GL_NEAREST, LINEAR = GL_LINEAR,
        // filtering (with mipmap)
        NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST, NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR, LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR, LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
        // input formats
        RED = GL_RED, RG = GL_RG, RGB = GL_RGB, BGR = GL_BGR, RGBA = GL_RGBA, BGRA = GL_BGRA,
        // internal formats
        COMPRESSED_RED = GL_COMPRESSED_RED, COMPRESSED_RG = GL_COMPRESSED_RG, COMPRESSED_RGB = GL_COMPRESSED_RGB, COMPRESSED_RGBA = GL_COMPRESSED_RGBA, COMPRESSED_SRGB = GL_COMPRESSED_SRGB, COMPRESSED_SRGB_ALPHA = GL_COMPRESSED_SRGB_ALPHA, 
        DEPTH_COMPONENT = GL_DEPTH_COMPONENT, DEPTH_COMPONENT16 = GL_DEPTH_COMPONENT16, DEPTH_COMPONENT24 = GL_DEPTH_COMPONENT24, DEPTH_COMPONENT32 = GL_DEPTH_COMPONENT32, 
        R3_G3_B2 = GL_R3_G3_B2, RGB4 = GL_RGB4, RGB5 = GL_RGB5, RGB8 = GL_RGB8, RGB10 = GL_RGB10, RGB12 = GL_RGB12, RGB16 = GL_RGB16, 
        RGBA2 = GL_RGBA2, RGBA4 = GL_RGBA4, RGB5_A1 = GL_RGB5_A1, RGBA8 = GL_RGBA8, RGB10_A2 = GL_RGB10_A2, RGBA12 = GL_RGBA12, RGBA16 = GL_RGBA16, 
        SRGB = GL_SRGB, SRGB8 = GL_SRGB8, SRGB_ALPHA = GL_SRGB_ALPHA, SRGB8_ALPHA8 = GL_SRGB8_ALPHA8,
    };

    GLTexture() {
        glGenTextures(1, &m_id);
        // some defaults
        switch (Dim) {
        case 3: set_wrapping(WRAP_Z, GL_CLAMP_TO_EDGE);
        case 2: set_wrapping(WRAP_Y, GL_CLAMP_TO_EDGE);
        case 1: set_wrapping(WRAP_X, GL_CLAMP_TO_EDGE);
        }
        set_min_filter(LINEAR);
        set_mag_filter(LINEAR);
    }
    ~GLTexture() {
        glDeleteTextures(1, &m_id);
    };

    void bind() const {
        glBindTexture(texture_type(), m_id);
    }
    void unbind() const {
        glBindTexture(texture_type(), 0);
    }

    template <typename T>
    void set_data(T* data, unsigned int format, size_t width, size_t height = 0, size_t depth = 0) {
        bind();
        unsigned int gl_type = gl_type_convert<T>();
        if constexpr(Dim == 1)
            glTexImage1D(GL_TEXTURE_1D, 0, m_internal_format, width, 0, format, gl_type, data);
        else if constexpr(Dim == 2)
            glTexImage2D(GL_TEXTURE_2D, 0, m_internal_format, width, height, 0, format, gl_type, data);
        else if constexpr(Dim == 3)
            glTexImage3D(GL_TEXTURE_3D, 0, m_internal_format, width, height, depth, 0, format, gl_type, data);

        if (m_mipmapped)
            generate_mipmap();
    }

    // Settings

    // what the gpu gets?
    void set_internal_format(unsigned int format) {
        m_internal_format = format;
    }

    void generate_mipmap(bool enable = true) {
        if (enable) {
            m_mipmapped = enable;
            bind();
            glGenerateMipmap(texture_type());
        }
    }

    void set_min_filter(unsigned int mode) const {
        bind();
        glTexParameteri(texture_type(), GL_TEXTURE_MIN_FILTER, mode);
    }

    void set_min_filter(unsigned int main, unsigned int mipmap) const {
        bind();
        unsigned int mode = 0;
        if (main == GLTexture::NEAREST && mipmap == GLTexture::NEAREST)
            mode = GL_NEAREST_MIPMAP_NEAREST;
        else if (main == GLTexture::NEAREST && mipmap == GLTexture::LINEAR)
            mode = GL_NEAREST_MIPMAP_LINEAR;
        else if (main == GLTexture::LINEAR  && mipmap == GLTexture::LINEAR)
            mode = GL_LINEAR_MIPMAP_LINEAR;
        else if (main == GLTexture::LINEAR  && mipmap == GLTexture::NEAREST)
            mode = GL_LINEAR_MIPMAP_NEAREST;

        glTexParameteri(texture_type(), GL_TEXTURE_MIN_FILTER, mode);
    }

    void set_mag_filter(unsigned int mode) const {
        bind();
        glTexParameteri(texture_type(), GL_TEXTURE_MAG_FILTER, mode);
    }

    void set_wrapping(unsigned int dim, unsigned int mode) const {
        bind();
        glTexParameterf(texture_type(), dim, mode);
    }

    void set_border_color(glm::vec3 color) const {
        set_border_color(glm::vec4(color, 1.0));
    }
    void set_border_color(glm::vec4 color) const {
        bind();
        glTexParameterfv(texture_type(), GL_TEXTURE_BORDER_COLOR, color);
    }

private:
    const unsigned int texture_type() const {
        if constexpr(Dim == 1)
            return GL_TEXTURE_1D;
        else if constexpr(Dim == 2)
            return GL_TEXTURE_2D;
        else if constexpr(Dim == 3)
            return GL_TEXTURE_3D;
    }

};