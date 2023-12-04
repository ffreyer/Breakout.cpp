#pragma once

#include <exception>
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
        // internal formats
        COMPRESSED_RED = GL_COMPRESSED_RED, COMPRESSED_RG = GL_COMPRESSED_RG, COMPRESSED_RGB = GL_COMPRESSED_RGB, COMPRESSED_RGBA = GL_COMPRESSED_RGBA, COMPRESSED_SRGB = GL_COMPRESSED_SRGB, COMPRESSED_SRGB_ALPHA = GL_COMPRESSED_SRGB_ALPHA, 
        DEPTH_COMPONENT = GL_DEPTH_COMPONENT, DEPTH_COMPONENT16 = GL_DEPTH_COMPONENT16, DEPTH_COMPONENT24 = GL_DEPTH_COMPONENT24, DEPTH_COMPONENT32 = GL_DEPTH_COMPONENT32, 
        R3_G3_B2 = GL_R3_G3_B2, RGB4 = GL_RGB4, RGB5 = GL_RGB5, RGB8 = GL_RGB8, RGB10 = GL_RGB10, RGB12 = GL_RGB12, RGB16 = GL_RGB16, 
        RGBA2 = GL_RGBA2, RGBA4 = GL_RGBA4, RGB5_A1 = GL_RGB5_A1, RGBA8 = GL_RGBA8, RGB10_A2 = GL_RGB10_A2, RGBA12 = GL_RGBA12, RGBA16 = GL_RGBA16, 
        SRGB = GL_SRGB, SRGB8 = GL_SRGB8, SRGB_ALPHA = GL_SRGB_ALPHA, SRGB8_ALPHA8 = GL_SRGB8_ALPHA8,
    };

    AbstractGLTexture(GLenum texture_type) : m_texture_type(texture_type) {
        glGenTextures(1, &m_id);
    }

    ~AbstractGLTexture() {
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
    