#include "GLTexture.hpp"

#include <iostream>

// AbstractGLTexture

AbstractGLTexture::AbstractGLTexture(GLenum texture_type) : m_texture_type(texture_type) {
    glGenTextures(1, &m_id);
}

AbstractGLTexture::~AbstractGLTexture() {
    glDeleteTextures(1, &m_id);
};

void AbstractGLTexture::bind() const {
    glActiveTexture(m_slot);
    glBindTexture(m_texture_type, m_id);
}

void AbstractGLTexture::unbind() const {
    glBindTexture(m_texture_type, 0);
}

unsigned int AbstractGLTexture::get_id() const {
    return m_id;
}


void AbstractGLTexture::set_slot(uint32_t slot) {
    int max_slots;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_slots);
    if (slot < max_slots)
        // TODO: maybe do this with a lookup table instead for safety?
        m_slot = GL_TEXTURE0 + slot;
    else
        throw std::invalid_argument("Invalid texture slot.");
}

// Settings
void AbstractGLTexture::set_min_filter(GLenum mode) const {
    bind();
    glTexParameteri(m_texture_type, GL_TEXTURE_MIN_FILTER, mode);
}

void AbstractGLTexture::set_min_filter(GLenum main, GLenum mipmap) const {
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

void AbstractGLTexture::set_mag_filter(GLenum mode) const {
    bind();
    glTexParameteri(m_texture_type, GL_TEXTURE_MAG_FILTER, mode);
}

void AbstractGLTexture::set_wrapping(GLenum dim, GLenum mode) const {
    bind();
    glTexParameterf(m_texture_type, dim, mode);
}

void AbstractGLTexture::set_border_color(glm::vec3 color) const {
    set_border_color(glm::vec4(color, 1.0));
}

void AbstractGLTexture::set_border_color(glm::vec4 color) const {
    bind();
    glTexParameterfv(m_texture_type, GL_TEXTURE_BORDER_COLOR, (const float*) &color);
}

void AbstractGLTexture::set_internal_format(GLenum format) {
    m_internal_format = format;
}

void AbstractGLTexture::generate_mipmap(bool enable) {
    if (enable) {
        m_mipmapped = enable;
        bind();
        glGenerateMipmap(m_texture_type);
    }
}

void AbstractGLTexture::resize(size_t width, size_t height, size_t depth) const {
    bind();
    GLenum format, type;
    switch (m_internal_format) {
    case GL_DEPTH24_STENCIL8:
        format = GL_DEPTH_STENCIL;
        type = GL_UNSIGNED_INT_24_8;
        break;
    case GL_DEPTH_COMPONENT:
        format = GL_DEPTH_COMPONENT;
        type = GL_FLOAT;
        break;
    // case GL_DEPTH32F_STENCIL8: // TODO:
    default:
        format = GL_RED;
        type = GL_UNSIGNED_BYTE;
    }

    if (height == 0)
        glTexImage1D(GL_TEXTURE_1D, 0, m_internal_format, width, 0, format, type, nullptr);
    else if (depth == 0)
        glTexImage2D(GL_TEXTURE_2D, 0, m_internal_format, width, height, 0, format, type, nullptr);
    else
        glTexImage3D(GL_TEXTURE_3D, 0, m_internal_format, width, height, depth, 0, format, type, nullptr);
}

// GLTexture

GLTexture::GLTexture(unsigned int dims)
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

void GLTexture::set_data(GLenum gl_type, void* data, GLenum format, size_t width, size_t height, size_t depth) {
    bind();
    if (m_dimensions == 1)
        glTexImage1D(GL_TEXTURE_1D, 0, m_internal_format, width, 0, format, gl_type, data);
    else if (m_dimensions == 2)
        glTexImage2D(GL_TEXTURE_2D, 0, m_internal_format, width, height, 0, format, gl_type, data);
    else if (m_dimensions == 3)
        glTexImage3D(GL_TEXTURE_3D, 0, m_internal_format, width, height, depth, 0, format, gl_type, data);

    if (m_mipmapped)
        generate_mipmap();
}

// Cubemap

GLCubeMap::GLCubeMap() : AbstractGLTexture(GL_TEXTURE_CUBE_MAP)
{
    set_wrapping(WRAP_X, GL_CLAMP_TO_EDGE);
    set_wrapping(WRAP_Y, GL_CLAMP_TO_EDGE);
    set_wrapping(WRAP_Z, GL_CLAMP_TO_EDGE);
    set_min_filter(LINEAR);
    set_mag_filter(LINEAR);
}
    
void GLCubeMap::set_data(GLenum gl_type, void* data, GLenum format, size_t width, size_t height, GLenum side) {
    bind();
    glTexImage2D(side, 0, m_internal_format, width, height, 0, format, gl_type, data);

    if (m_mipmapped)
        generate_mipmap();
}