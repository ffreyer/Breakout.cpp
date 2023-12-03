
#include "VertexArray.hpp"

// GLVertexBuffer

GLVertexBuffer::GLVertexBuffer(size_t size, unsigned int mode)
    : m_size(size), m_mode(mode)
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, m_size, nullptr, m_mode);
}

GLVertexBuffer::GLVertexBuffer(void* vertices, size_t size, unsigned int mode)
    : m_size(size), m_mode(mode)
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, m_size, vertices, m_mode);
}

GLVertexBuffer::~GLVertexBuffer() {
    glDeleteBuffers(1, &m_id);
}

void GLVertexBuffer::set_data(const void* vertices, unsigned int size) {
    bind();
    m_size = size;
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_size, vertices);
}

void GLVertexBuffer::set_layout(const GLBufferLayout& layout) {
    m_layout = layout;
}

void GLVertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void GLVertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLBufferLayout GLVertexBuffer::get_layout() const {
	return m_layout;
}


// GLIndexBuffer

GLIndexBuffer::GLIndexBuffer(size_t size, unsigned int mode)
    : m_size(size), m_mode(mode)
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_size, nullptr, m_mode);
}

GLIndexBuffer::GLIndexBuffer(uint32_t* indices, size_t size, unsigned int mode)
    : m_size(size), m_mode(mode)
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_size, indices, m_mode);
}

GLIndexBuffer::~GLIndexBuffer() {
    glDeleteBuffers(1, &m_id);
}

void GLIndexBuffer::set(const uint32_t * indices, size_t size) {
    bind();
    m_size = size;
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_size, indices);
}

void GLIndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void GLIndexBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
