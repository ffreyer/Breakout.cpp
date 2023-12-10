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
    GLenum m_slot = GL_TEXTURE0;
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

    AbstractGLTexture(GLenum texture_type);
    virtual ~AbstractGLTexture();

    virtual void bind() const;
    virtual void unbind() const;
    unsigned int get_id() const;

    void set_slot(uint8_t slot);
    void set_min_filter(GLenum mode) const; // combined filter type
    void set_min_filter(GLenum main, GLenum mipmap) const; // separate filter types
    void set_mag_filter(GLenum mode) const;
    void set_wrapping(GLenum dim, GLenum mode) const;
    void set_border_color(glm::vec3 color) const;
    void set_border_color(glm::vec4 color) const;
    void set_internal_format(GLenum format);
    void generate_mipmap(bool enable = true);

    virtual void resize(size_t width, size_t height = 0, size_t depth = 0) const;

    friend class GLFramebuffer;
};


class GLTexture : public AbstractGLTexture {
private:
    unsigned char m_dimensions;
public:
    GLTexture(unsigned int dims);

    template <typename T>
    void set_data(T* data, GLenum format, size_t width, size_t height = 0, size_t depth = 0) {
        GLenum gl_type = gl_type_convert<T>();
        set_data(gl_type, (void*) data, format, width, height, depth);
    }

	template <typename T, size_t N>
	void set_data(std::array<T, N>& data, GLenum format, size_t width, size_t height = 0, size_t depth = 0) {
		set_data(data.data(), format, width, height, depth);
	}

	template <typename T>
	void set_data(std::vector<T>& data, GLenum format, size_t width, size_t height = 0, size_t depth = 0) {
		set_data(data.data(), format, width, height, depth);
	}

    void set_data(GLenum gl_type, void* data, GLenum format, size_t width, size_t height = 0, size_t depth = 0);

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

    GLCubeMap();

	template <typename T, size_t N>
	void set_data(std::array<T, N>& data, GLenum format, size_t width, size_t height, GLenum side) {
		set_data(data.data(), format, width, height, side);
	}
    
    template <typename T>
	void set_data(std::vector<T>& data, GLenum format, size_t width, size_t height, GLenum side) {
		set_data(data.data(), format, width, height, side);
	}

    template <typename T>
    void set_data(T* data, GLenum format, size_t width, size_t height, GLenum side) {
        GLenum gl_type = gl_type_convert<T>();
        set_data(gl_type, (void*) data, format, width, height, side);
    }

    void set_data(GLenum gl_type, void* data, GLenum format, size_t width, size_t height, GLenum side);
};

[[maybe_unused]] static GLenum channels_to_gl_type(int channels) {
    switch (channels) {
        case 1: return AbstractGLTexture::RED;
        case 2: return AbstractGLTexture::RG;
        case 3: return AbstractGLTexture::RGB;
        case 4: return AbstractGLTexture::RGBA;
    }
    return 0;
}
    