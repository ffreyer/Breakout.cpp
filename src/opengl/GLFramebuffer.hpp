#pragma once

#include <glad/gl.h>

#include <vector>
#include <exception>

#include "GLTexture.hpp"

class GLFramebuffer {
private:
    unsigned int m_id = 0;
    size_t m_width = 800, m_height = 600, m_counter = 0;
    std::vector<GLenum> m_attachments;
    std::vector<AbstractGLTexture> m_textures;

public:
    enum : GLenum {
        READ = GL_READ_FRAMEBUFFER, DRAW = GL_DRAW_FRAMEBUFFER,
        // Attachments
        COLOR0 = GL_COLOR_ATTACHMENT0, COLOR1 = GL_COLOR_ATTACHMENT1, COLOR2 = GL_COLOR_ATTACHMENT2, COLOR3 = GL_COLOR_ATTACHMENT3, COLOR4 = GL_COLOR_ATTACHMENT4, COLOR5 = GL_COLOR_ATTACHMENT5, COLOR6 = GL_COLOR_ATTACHMENT6, COLOR7 = GL_COLOR_ATTACHMENT7, COLOR8 = GL_COLOR_ATTACHMENT8, COLOR9 = GL_COLOR_ATTACHMENT9,
        COLOR10 = GL_COLOR_ATTACHMENT10, COLOR11 = GL_COLOR_ATTACHMENT11, COLOR12 = GL_COLOR_ATTACHMENT12, COLOR13 = GL_COLOR_ATTACHMENT13, COLOR14 = GL_COLOR_ATTACHMENT14, COLOR15 = GL_COLOR_ATTACHMENT15, COLOR16 = GL_COLOR_ATTACHMENT16, COLOR17 = GL_COLOR_ATTACHMENT17, COLOR18 = GL_COLOR_ATTACHMENT18, COLOR19 = GL_COLOR_ATTACHMENT19,
        COLOR20 = GL_COLOR_ATTACHMENT20, COLOR21 = GL_COLOR_ATTACHMENT21, COLOR22 = GL_COLOR_ATTACHMENT22, COLOR23 = GL_COLOR_ATTACHMENT23, COLOR24 = GL_COLOR_ATTACHMENT24, COLOR25 = GL_COLOR_ATTACHMENT25, COLOR26 = GL_COLOR_ATTACHMENT26, COLOR27 = GL_COLOR_ATTACHMENT27, COLOR28 = GL_COLOR_ATTACHMENT28, COLOR29 = GL_COLOR_ATTACHMENT29,
        COLOR30 = GL_COLOR_ATTACHMENT30, COLOR31 = GL_COLOR_ATTACHMENT31,
        DEPTH_STENCIL = GL_DEPTH_STENCIL_ATTACHMENT, DEPTH = GL_DEPTH_ATTACHMENT, STENCIL = GL_STENCIL_ATTACHMENT,
        // internal format (same as texture)
        RED = GL_RED, RG = GL_RG, RGB = GL_RGB, BGR = GL_BGR, RGBA = GL_RGBA, BGRA = GL_BGRA,
        RGBA32F = GL_RGBA32F, RGBA32I = GL_RGBA32I, RGBA32UI = GL_RGBA32UI, RGBA16 = GL_RGBA16, RGBA16F = GL_RGBA16F, RGBA16I = GL_RGBA16I, RGBA16UI = GL_RGBA16UI, RGBA8 = GL_RGBA8, RGBA8UI = GL_RGBA8UI, SRGB8_ALPHA8 = GL_SRGB8_ALPHA8, RGB10_A2UI = GL_RGB10_A2UI, RGB10_A2 = GL_RGB10_A2, 
        R11F_G11F_B10F = GL_R11F_G11F_B10F, RGB16I = GL_RGB16I, RGB16UI = GL_RGB16UI, RGB32F = GL_RGB32F, RGB32I = GL_RGB32I, RGB32UI = GL_RGB32UI, RGB16F = GL_RGB16F, RGB16 = GL_RGB16, RGB8 = GL_RGB8, RGB8I = GL_RGB8I, RGB8UI = GL_RGB8UI, SRGB8 = GL_SRGB8, RGB9_E5 = GL_RGB9_E5, 
        RG32F = GL_RG32F, RG32I = GL_RG32I, RG32UI = GL_RG32UI, RG16 = GL_RG16, RG16F = GL_RG16F, RG8 = GL_RG8, RG8I = GL_RG8I, RG8UI = GL_RG8UI, 
        R32F = GL_R32F, R32I = GL_R32I, R32UI = GL_R32UI, R16F = GL_R16F, R16I = GL_R16I, R16UI = GL_R16UI, R8 = GL_R8, R8I = GL_R8I, R8UI = GL_R8UI, 
        RGB16_SNORM = GL_RGB16_SNORM, RGB8_SNORM = GL_RGB8_SNORM, RGBA16_SNORM = GL_RGBA16_SNORM, RGBA8_SNORM = GL_RGBA8_SNORM, RG16_SNORM = GL_RG16_SNORM, RG8_SNORM = GL_RG8_SNORM, R16_SNORM = GL_R16_SNORM, R8_SNORM = GL_R8_SNORM, 
        COMPRESSED_RG_RGTC2 = GL_COMPRESSED_RG_RGTC2, COMPRESSED_SIGNED_RG_RGTC2 = GL_COMPRESSED_SIGNED_RG_RGTC2, COMPRESSED_RED_RGTC1 = GL_COMPRESSED_RED_RGTC1, COMPRESSED_SIGNED_RED_RGTC1 = GL_COMPRESSED_SIGNED_RED_RGTC1, 
        DEPTH_COMPONENT32F = GL_DEPTH_COMPONENT32F, DEPTH_COMPONENT24 = GL_DEPTH_COMPONENT24, DEPTH_COMPONENT16 = GL_DEPTH_COMPONENT16, DEPTH32F_STENCIL8 = GL_DEPTH32F_STENCIL8, DEPTH24_STENCIL8 = GL_DEPTH24_STENCIL8,
    };

    GLFramebuffer() {
        glGenFramebuffers(1, &m_id);
        m_textures.reserve(34);
    }
    ~GLFramebuffer() {
        glDeleteFramebuffers(1, &m_id);
    }

    void bind(GLenum mode = GL_FRAMEBUFFER) const {
        glBindFramebuffer(mode, m_id);
    }

    void unbind(GLenum mode = GL_FRAMEBUFFER) const {
        glBindFramebuffer(mode, 0);
    }

    void attach_color_texture(GLenum internal_format = GL_RGB) {
        attach_texture(GL_COLOR_ATTACHMENT0 + m_counter, GL_TEXTURE_2D, internal_format);
        m_counter++;
    }
    void attach_depth_texture() {
        attach_texture(GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, GL_DEPTH24_STENCIL8);
    }
    void attach_texture(GLenum attachment, GLenum texture_type, GLenum internal_format) {
        m_attachments.push_back(attachment);
        m_textures.emplace_back(texture_type);
        AbstractGLTexture& tex = m_textures.back();
        tex.set_internal_format(internal_format);
        tex.resize(m_width, m_height);
        tex.set_min_filter(AbstractGLTexture::LINEAR);
        tex.set_mag_filter(AbstractGLTexture::LINEAR);
        tex.set_wrapping(AbstractGLTexture::WRAP_X, AbstractGLTexture::CLAMP_TO_EDGE);
        tex.set_wrapping(AbstractGLTexture::WRAP_Y, AbstractGLTexture::CLAMP_TO_EDGE);
        tex.set_wrapping(AbstractGLTexture::WRAP_Z, AbstractGLTexture::CLAMP_TO_EDGE);
        tex.unbind();
        attach(m_attachments.back(), tex);
        if (!verify())
            // TODO: improve exceptions
            throw std::invalid_argument("Framebuffer error occured!");
    }

    AbstractGLTexture& get(uint8_t index) {
        return m_textures[index];
    }

    void resize(size_t width, size_t height) {
        if (m_width != width || m_height != height) {
            m_width = width;
            m_height = height;

            for (auto& tex : m_textures) {
                tex.resize(m_width, m_height);
            }
        }
    }

    GLenum status() const {
        bind();
        return glCheckFramebufferStatus(GL_FRAMEBUFFER);
    }
    bool verify() const {
        return status() == GL_FRAMEBUFFER_COMPLETE;
    }

private:
    void attach(GLenum attachment, AbstractGLTexture& tex, int mipmap_level = 0) const {
        bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, tex.m_texture_type, tex.m_id, mipmap_level);
    }
    // void attach(GLenum attachment, GLRenderbuffer& rb, int mipmap_level = 0) const {
    //     glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rb.m_id);
    // }
};


// TODO:

/*
class GLRenderbuffer {
// faster because they are simpler than textures (native format, no read)
private:
    unsigned int m_id;
    GLenum m_internal_format = GL_DEPTH24_STENCIL8;

public:
    enum : GLenum {
        DPETH24_STENCIL8 = GL_DEPTH24_STENCIL8,
    };

    GLRenderbuffer() {
        glCreateRenderbuffers(1, &m_id);
    }
    ~GLRenderbuffer() {
        glDeleteRenderbuffers(1, &m_id);
    }

    void bind() {
        glBindRenderbuffer(GL_RENDERBUFFER, m_id);
    }
    void unbind() {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    void set_internal_format(GLenum format) {
        m_internal_format = format;
    }

    void resize(size_t w, size_t h) {
        glRenderbufferStorage(GL_RENDERBUFFER, m_internal_format, w, h);
    }

    friend class GLFramebuffer;
};
*/