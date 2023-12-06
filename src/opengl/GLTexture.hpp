#pragma once

#include <stdexcept>
#include <array>
#include <vector>

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <stb_image.h>

#include "GLTypeConvert.hpp"

class AbstractGLTexture {
protected:
    const GLenum m_texture_type;
    unsigned int m_id = 0;
    bool m_mipmapped = false;
    GLenum m_internal_format = GL_RGBA;

public:
    enum : GLenum {
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
        RED_INTEGER = GL_RED_INTEGER, RG_INTEGER = GL_RG_INTEGER, RGB_INTEGER = GL_RGB_INTEGER, RGBA_INTEGER = GL_RGBA_INTEGER,
        // internal formats
        RGBA32F = GL_RGBA32F, RGBA32I = GL_RGBA32I, RGBA32UI = GL_RGBA32UI, RGBA16 = GL_RGBA16, RGBA16F = GL_RGBA16F, RGBA16I = GL_RGBA16I, RGBA16UI = GL_RGBA16UI, RGBA8 = GL_RGBA8, RGBA8UI = GL_RGBA8UI, SRGB8_ALPHA8 = GL_SRGB8_ALPHA8, RGB10_A2UI = GL_RGB10_A2UI, RGB10_A2 = GL_RGB10_A2, 
        R11F_G11F_B10F = GL_R11F_G11F_B10F, RGB16I = GL_RGB16I, RGB16UI = GL_RGB16UI, RGB32F = GL_RGB32F, RGB32I = GL_RGB32I, RGB32UI = GL_RGB32UI, RGB16F = GL_RGB16F, RGB16 = GL_RGB16, RGB8 = GL_RGB8, RGB8I = GL_RGB8I, RGB8UI = GL_RGB8UI, SRGB8 = GL_SRGB8, RGB9_E5 = GL_RGB9_E5, 
        RG32F = GL_RG32F, RG32I = GL_RG32I, RG32UI = GL_RG32UI, RG16 = GL_RG16, RG16F = GL_RG16F, RG8 = GL_RG8, RG8I = GL_RG8I, RG8UI = GL_RG8UI, 
        R32F = GL_R32F, R32I = GL_R32I, R32UI = GL_R32UI, R16F = GL_R16F, R16I = GL_R16I, R16UI = GL_R16UI, R8 = GL_R8, R8I = GL_R8I, R8UI = GL_R8UI, 
        RGB16_SNORM = GL_RGB16_SNORM, RGB8_SNORM = GL_RGB8_SNORM, RGBA16_SNORM = GL_RGBA16_SNORM, RGBA8_SNORM = GL_RGBA8_SNORM, RG16_SNORM = GL_RG16_SNORM, RG8_SNORM = GL_RG8_SNORM, R16_SNORM = GL_R16_SNORM, R8_SNORM = GL_R8_SNORM, 
        COMPRESSED_RG_RGTC2 = GL_COMPRESSED_RG_RGTC2, COMPRESSED_SIGNED_RG_RGTC2 = GL_COMPRESSED_SIGNED_RG_RGTC2, COMPRESSED_RED_RGTC1 = GL_COMPRESSED_RED_RGTC1, COMPRESSED_SIGNED_RED_RGTC1 = GL_COMPRESSED_SIGNED_RED_RGTC1, 
        DEPTH_COMPONENT32F = GL_DEPTH_COMPONENT32F, DEPTH_COMPONENT24 = GL_DEPTH_COMPONENT24, DEPTH_COMPONENT16 = GL_DEPTH_COMPONENT16, DEPTH32F_STENCIL8 = GL_DEPTH32F_STENCIL8, DEPTH24_STENCIL8 = GL_DEPTH24_STENCIL8,
    };

    AbstractGLTexture(GLenum texture_type) : m_texture_type(texture_type) {
        glGenTextures(1, &m_id);
    }

    virtual ~AbstractGLTexture() {
        glDeleteTextures(1, &m_id);
    };

    virtual void bind() const {
        glBindTexture(m_texture_type, m_id);
    }
    virtual void unbind() const {
        glBindTexture(m_texture_type, 0);
    }

    // Settings
    virtual void set_min_filter(GLenum mode) const {
        bind();
        glTexParameteri(m_texture_type, GL_TEXTURE_MIN_FILTER, mode);
    }

    virtual void set_min_filter(GLenum main, GLenum mipmap) const {
        bind();
        GLenum mode = 0;
        if (main == NEAREST && mipmap == NEAREST)
            mode = GL_NEAREST_MIPMAP_NEAREST;
        else if (main == NEAREST && mipmap == LINEAR)
            mode = GL_NEAREST_MIPMAP_LINEAR;
        else if (main == LINEAR  && mipmap == LINEAR)
            mode = GL_LINEAR_MIPMAP_LINEAR;
        else if (main == LINEAR  && mipmap == NEAREST)
            mode = GL_LINEAR_MIPMAP_NEAREST;

        glTexParameteri(m_texture_type, GL_TEXTURE_MIN_FILTER, mode);
    }

    virtual void set_mag_filter(GLenum mode) const {
        bind();
        glTexParameteri(m_texture_type, GL_TEXTURE_MAG_FILTER, mode);
    }

    virtual void set_wrapping(GLenum dim, GLenum mode) const {
        bind();
        glTexParameterf(m_texture_type, dim, mode);
    }

    virtual void set_border_color(glm::vec3 color) const {
        set_border_color(glm::vec4(color, 1.0));
    }
    virtual void set_border_color(glm::vec4 color) const {
        bind();
        glTexParameterfv(m_texture_type, GL_TEXTURE_BORDER_COLOR, (const float*) &color);
    }

    // format on gpu
    void set_internal_format(GLenum format) {
        m_internal_format = format;
    }

    void generate_mipmap(bool enable = true) {
        if (enable) {
            m_mipmapped = enable;
            bind();
            glGenerateMipmap(m_texture_type);
        }
    }
};


class GLTexture : public AbstractGLTexture {
private:
    unsigned char m_dimensions;
public:
    GLTexture(unsigned int dims)
        : AbstractGLTexture(GLTexture::texture_type(dims)), m_dimensions(dims)
    {
        // some defaults
        switch (dims) {
        case 3: set_wrapping(WRAP_Z, GL_CLAMP_TO_EDGE);
        case 2: set_wrapping(WRAP_Y, GL_CLAMP_TO_EDGE);
        case 1: set_wrapping(WRAP_X, GL_CLAMP_TO_EDGE);
        }
        set_min_filter(LINEAR);
        set_mag_filter(LINEAR);
    }

    template <typename T>
    void set_data(T* data, GLenum format, size_t width, size_t height = 0, size_t depth = 0) {
        bind();
        GLenum gl_type = gl_type_convert<T>();
        if (m_dimensions == 1)
            glTexImage1D(GL_TEXTURE_1D, 0, m_internal_format, width, 0, format, gl_type, data);
        else if (m_dimensions == 2)
            glTexImage2D(GL_TEXTURE_2D, 0, m_internal_format, width, height, 0, format, gl_type, data);
        else if (m_dimensions == 3)
            glTexImage3D(GL_TEXTURE_3D, 0, m_internal_format, width, height, depth, 0, format, gl_type, data);

        if (m_mipmapped)
            generate_mipmap();
    }

	template <typename T, size_t N>
	void set_data(std::array<T, N>& data, GLenum format, size_t width, size_t height = 0, size_t depth = 0) {
		set_data(data.data(), format, width, height, depth);
	}
	template <typename T>
	void set_data(std::vector<T>& data, GLenum format, size_t width, size_t height = 0, size_t depth = 0) {
		set_data(data.data(), format, width, height, depth);
	}

private:
    static GLenum texture_type(unsigned char dims) {
        if (dims == 1)
            return GL_TEXTURE_1D;
        else if (dims == 2)
            return GL_TEXTURE_2D;
        else if (dims == 3)
            return GL_TEXTURE_3D;
        else {
            throw std::invalid_argument("Invalid dimension for GLTexture.");
        }
    }
};


class GLCubeMap : public AbstractGLTexture {
public:
    enum : GLenum {
        RIGHT  = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        LEFT   = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        TOP    = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        BOTTOM = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        BACK   = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        FRONT  = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    };

    GLCubeMap() : AbstractGLTexture(GL_TEXTURE_CUBE_MAP)
    {
        set_wrapping(WRAP_X, GL_CLAMP_TO_EDGE);
        set_wrapping(WRAP_Y, GL_CLAMP_TO_EDGE);
        set_wrapping(WRAP_Z, GL_CLAMP_TO_EDGE);
        set_min_filter(LINEAR);
        set_mag_filter(LINEAR);
    }
    
    template <typename T>
    void set_data(T* data, GLenum format, size_t width, size_t height, GLenum side) {
        bind();
        GLenum gl_type = gl_type_convert<T>();
        glTexImage2D(side, 0, m_internal_format, width, height, 0, format, gl_type, data);

        if (m_mipmapped)
            generate_mipmap();
    }

	template <typename T, size_t N>
	void set_data(std::array<T, N>& data, GLenum format, size_t width, size_t height, GLenum side) {
		set_data(data.data(), format, width, height, side);
	}
	template <typename T>
	void set_data(std::vector<T>& data, GLenum format, size_t width, size_t height, GLenum side) {
		set_data(data.data(), format, width, height, side);
	}
};

static GLenum channels_to_gl_type(int channels) {
    switch (channels) {
        case 1: return AbstractGLTexture::RED;
        case 2: return AbstractGLTexture::RG;
        case 3: return AbstractGLTexture::RGB;
        case 4: return AbstractGLTexture::RGBA;
    }
    return 0;
}
    