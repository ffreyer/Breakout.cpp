
#include "GLVertexArray.hpp"

// Generic buffer
GLBuffer::GLBuffer(GLenum buffer_type, void* vertices, size_t bytesize, unsigned int mode)
    : m_buffer_type(buffer_type), m_mode(mode), m_size(bytesize)
{
    glGenBuffers(1, &m_id);
    glBindBuffer(m_buffer_type, m_id);
    glBufferData(m_buffer_type, m_size, vertices, m_mode);
}

GLBuffer::~GLBuffer() {
    glDeleteBuffers(1, &m_id);
}

void GLBuffer::set_data(const void* vertices, unsigned int bytesize) {
    bind();
    m_size = bytesize;
    glBufferSubData(m_buffer_type, 0, m_size, vertices);
}

void GLBuffer::bind() const {
    glBindBuffer(m_buffer_type, m_id);
}

void GLBuffer::unbind() const {
    glBindBuffer(m_buffer_type, 0);
}

void GLBuffer::bind_buffer_base(uint32_t index) const {
	glBindBufferBase(m_buffer_type, index, m_id);
}


// GLVertexBuffer

void GLVertexBuffer::set_layout(const GLBufferLayout& layout) {
    m_layout = layout;
}

GLBufferLayout GLVertexBuffer::get_layout() const {
	return m_layout;
}

// GLVertexArray

GLVertexArray::GLVertexArray()
    : m_buffer_idx(0)
{
    glGenVertexArrays(1, &m_id);
}

GLVertexArray::~GLVertexArray() {
    glDeleteVertexArrays(1, &m_id);
}

void GLVertexArray::set(std::shared_ptr<GLIndexBuffer> indices) {
    m_indices = indices;
}

void GLVertexArray::push(std::shared_ptr<GLVertexBuffer> buffer) {
    bind();
	buffer->bind();
	GLBufferLayout layout = buffer->get_layout();

	for (const GLBufferElement& element : layout) {
		switch (element.type) {
			case GLType::Bool:
			case GLType::Int:
			case GLType::Int2:
			case GLType::Int3:
			case GLType::Int4:
			case GLType::Short:
			case GLType::Short2:
			case GLType::Short3:
			case GLType::Short4:
			case GLType::UShort:
			case GLType::UShort2:
			case GLType::UShort3:
			case GLType::UShort4:
			{
				glEnableVertexAttribArray(m_buffer_idx);
				glVertexAttribIPointer(
					m_buffer_idx,                   // layout location in shader
					element.component_count(),      // number of values in "vertex" element
					gl_native_type(element.type),   // GL<type>
					layout.get_stride(),            // length of "vertex"
					(const void*)element.offset     // offset of first value in "vertex" element
				);
				m_buffer_idx++;
				break;
			}
			case GLType::Float: 
			case GLType::Float2:
			case GLType::Float3:
			case GLType::Float4:
			{
				glEnableVertexAttribArray(m_buffer_idx);
				glVertexAttribPointer(
					m_buffer_idx,
					element.component_count(),
					gl_native_type(element.type),
					element.normalize ? GL_TRUE : GL_FALSE,  // normalize first, or convert to fixed point data directly (if applicable?)
					layout.get_stride(),
					(const void*)element.offset
				);
				m_buffer_idx++;
				break;
			}
			case GLType::Mat3:
			case GLType::Mat4:
			{
				size_t count = element.component_count();
				for (size_t i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(m_buffer_idx);
					glVertexAttribPointer(
						m_buffer_idx,
						count,
						gl_native_type(element.type),
						element.normalize ? GL_TRUE : GL_FALSE,
						layout.get_stride(),
						(const void*)(element.offset + sizeof(float) * count * i)
					);
					m_buffer_idx++;
				}
				break;
			}
			default: 
				std::cout << "Failed to convert unknown GLType" << std::endl;
		}
	}

	m_buffers.push_back(buffer);
}

void GLVertexArray::bind() const {
    glBindVertexArray(m_id);
	if (m_indices)
		m_indices->bind();
}

void GLVertexArray::update(size_t idx, void* data, size_t size) const {
    m_buffers[idx]->set_data(data, size);
}

void GLVertexArray::set_layout(size_t idx, const GLBufferLayout& layout) const {
	m_buffers[idx]->set_layout(layout);
}