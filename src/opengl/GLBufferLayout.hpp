#pragma once

#include <string>
#include <iostream>
#include <vector>

#include <glad/gl.h>

// Aliases for GL types so we can keep things seperated
enum class GLType{
	Bool,
	Int, Int2, Int3, Int4,
	Float, Float2, Float3, Float4,
	Mat3, Mat4
};

static unsigned int gltype_byte_size(GLType type) {
    switch (type) {
		case GLType::Bool: return 1;
		case GLType::Int:  return 4;
		case GLType::Int2: return 4 * 2;
		case GLType::Int3: return 4 * 3;
		case GLType::Int4: return 4 * 4;
		case GLType::Float:  return 4;
		case GLType::Float2: return 4 * 2;
		case GLType::Float3: return 4 * 3;
		case GLType::Float4: return 4 * 4;
		case GLType::Mat3: return 4 * 3 * 3;
		case GLType::Mat4: return 4 * 4 * 4;
	}
	std::cout << "GLType " << (int) type << " not recognized" << std::endl;
	return 0;
}

static unsigned int gltype_length(GLType type) {
	switch (type) {
        case GLType::Bool: return 1;
        case GLType::Int:  return 1;
        case GLType::Int2: return 2;
        case GLType::Int3: return 3;
        case GLType::Int4: return 4;
        case GLType::Float:  return 1;
        case GLType::Float2: return 2;
        case GLType::Float3: return 3;
        case GLType::Float4: return 4;
        case GLType::Mat3: return 3 * 3;
        case GLType::Mat4: return 4 * 4;
	}
	std::cout << "GLType " << (int)type << " not recognized" << std::endl;
	return 0;
}

static GLenum gl_native_type(GLType type) {
	switch (type) {
        case GLType::Bool: return GL_BOOL;
        case GLType::Int:  return GL_INT;
        case GLType::Int2: return GL_INT;
        case GLType::Int3: return GL_INT;
        case GLType::Int4: return GL_INT;
        case GLType::Float:  return GL_FLOAT;
        case GLType::Float2: return GL_FLOAT;
        case GLType::Float3: return GL_FLOAT;
        case GLType::Float4: return GL_FLOAT;
        case GLType::Mat3: return GL_FLOAT;
        case GLType::Mat4: return GL_FLOAT;
	}
	std::cout << "GLType " << (int)type << " not recognized";
	return 0;
}

// Information for one block of data in a vertex (* coming from one glBuffer object)
struct GLBufferElement {
    std::string name;   // for usability, e.g. position, normal
    GLType type;        // type information
    size_t size;        // byte size
    size_t offset;      // offset from start of vertex in bytes (set by GLBufferLayout)
    bool normalize;     // should opengl normalize this?

    GLBufferElement(const std::string& name, GLType type, bool normalize = false)
		: name(name), type(type), size(gltype_byte_size(type)), 
          offset(0), normalize(normalize)
	{
	}

	size_t component_count() const { return gltype_length(type); }
};

// Information on the Layout of a vertex (via GLBufferElement, * coming from one glBuffer object)
class GLBufferLayout {
private:
	std::vector<GLBufferElement> m_elements;
	size_t m_stride = 0; // byte size of a vertex data (* defined by one glBuffer object)

public:
	GLBufferLayout() = default;

	GLBufferLayout(std::initializer_list<GLBufferElement> elements)
		: m_elements(elements)
	{
		calculate_offsets_and_stride();
	}

	size_t get_stride() const { return m_stride; }

	std::vector<GLBufferElement>::iterator begin() { return m_elements.begin(); }
	std::vector<GLBufferElement>::iterator end() { return m_elements.end(); }
	std::vector<GLBufferElement>::const_iterator begin() const { return m_elements.begin(); }
	std::vector<GLBufferElement>::const_iterator end() const { return m_elements.end(); }

private:
	void calculate_offsets_and_stride() {
		size_t offset = 0;
		for (GLBufferElement& element : m_elements) {
			element.offset = offset;
			offset += element.size;
		}
		m_stride = (size_t)offset;
	}
};


namespace DefaultBufferLayout
{
	const GLBufferLayout Plain = GLBufferLayout({ 
		GLBufferElement("Position", GLType::Float3) 
	});

	const GLBufferLayout Normal = GLBufferLayout({
		GLBufferElement("Position", GLType::Float3),
		GLBufferElement("Normal",   GLType::Float3)
	});

	const GLBufferLayout NormalUV = GLBufferLayout({
		GLBufferElement("Position",           GLType::Float3),
		GLBufferElement("Normal",             GLType::Float3),
		GLBufferElement("Texture coordinate", GLType::Float2)
	});
}