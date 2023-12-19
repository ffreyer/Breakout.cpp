#pragma once

#include <array>
#include <vector>

#include <glad/gl.h>

#include "GLBufferLayout.hpp"

class GLBuffer {
protected:
	unsigned int m_id = 0;
	GLenum m_buffer_type;
	GLenum m_mode;
	uint32_t m_size;

public:
	enum : GLenum {
		STREAM_DRAW = GL_STREAM_DRAW, STREAM_READ = GL_STREAM_READ, STREAM_COPY = GL_STREAM_COPY, 
		STATIC_DRAW = GL_STATIC_DRAW, STATIC_READ = GL_STATIC_READ, STATIC_COPY = GL_STATIC_COPY, 
		DYNAMIC_DRAW = GL_DYNAMIC_DRAW, DYNAMIC_READ = GL_DYNAMIC_READ, DYNAMIC_COPY = GL_DYNAMIC_COPY
	};

	enum : GLenum {
		ARRAY_BUFFER = GL_ARRAY_BUFFER, 
		COPY_READ_BUFFER = GL_COPY_READ_BUFFER, 
		COPY_WRITE_BUFFER = GL_COPY_WRITE_BUFFER, 
		ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER, 
		PIXEL_PACK_BUFFER = GL_PIXEL_PACK_BUFFER, 
		PIXEL_UNPACK_BUFFER = GL_PIXEL_UNPACK_BUFFER, 
		TEXTURE_BUFFER = GL_TEXTURE_BUFFER, 
		TRANSFORM_FEEDBACK_BUFFER = GL_TRANSFORM_FEEDBACK_BUFFER, 
		UNIFORM_BUFFER = GL_UNIFORM_BUFFER,
	};

	GLBuffer(GLenum buffer_type, void* vertices, size_t bytesize, GLenum mode = GLBuffer::STREAM_DRAW);
	GLBuffer(GLenum buffer_type, size_t size, GLenum mode = STREAM_DRAW)
		: GLBuffer(buffer_type, nullptr, size, mode)
	{}
	template <typename T, size_t N>
	GLBuffer(GLenum buffer_type, std::array<T, N>& vertices, GLenum mode = GLBuffer::STREAM_DRAW)
		: GLBuffer(vertices.data(), N * sizeof(T), mode)
	{}
	template <typename T>
	GLBuffer(GLenum buffer_type, std::vector<T>& vertices, GLenum mode = GLBuffer::STREAM_DRAW)
		: GLBuffer(vertices.data(), vertices.size() * sizeof(T), mode)
	{}

	virtual ~GLBuffer();

	void set_data(const void* vertices, unsigned int size);
	void bind() const;
	void unbind() const;
	void bind_buffer_base(uint32_t index) const;
	uint32_t bytesize() const { return m_size; }
};

class GLVertexBuffer : public GLBuffer {
private:
	GLBufferLayout m_layout;

public:
	GLVertexBuffer(void* vertices, size_t bytesize, GLenum mode = GLBuffer::STREAM_DRAW)
		: GLBuffer(GLBuffer::ARRAY_BUFFER, vertices, bytesize, mode)
	{}
	GLVertexBuffer(size_t bytesize, GLenum mode = GLBuffer::STREAM_DRAW)
		: GLVertexBuffer(nullptr, bytesize, mode)
	{}
	template <typename T, size_t N>
	GLVertexBuffer(std::array<T, N>& vertices, GLenum mode = GLBuffer::STREAM_DRAW)
		: GLVertexBuffer(vertices.data(), N * sizeof(T), mode)
	{}
	template <typename T>
	GLVertexBuffer(std::vector<T>& vertices, GLenum mode = GLBuffer::STREAM_DRAW)
		: GLVertexBuffer(vertices.data(), vertices.size() * sizeof(T), mode)
	{}

	void set_layout(const GLBufferLayout& layout);
	GLBufferLayout get_layout() const;
};


class GLIndexBuffer : public GLBuffer {
public:
	GLIndexBuffer(uint32_t* indices, size_t bytesize, GLenum mode = GLBuffer::STATIC_DRAW)
		: GLBuffer(GLBuffer::ELEMENT_ARRAY_BUFFER, indices, bytesize, mode)
	{}
	GLIndexBuffer(size_t bytesize, GLenum mode = GLBuffer::STATIC_DRAW)
		: GLIndexBuffer(nullptr, bytesize, mode)
	{}
	template <size_t N>
	GLIndexBuffer(std::array<uint32_t, N>& vertices, GLenum mode = GLBuffer::STATIC_DRAW)
		: GLIndexBuffer(vertices.data(), N * sizeof(uint32_t), mode)
	{}
	GLIndexBuffer(std::vector<uint32_t>& vertices, GLenum mode = GLBuffer::STATIC_DRAW) 
		: GLIndexBuffer(vertices.data(), vertices.size() * sizeof(uint32_t), mode) 
	{}

	uint32_t count() const { return m_size / sizeof(uint32_t); }
};


class GLVertexArray {
private:
	unsigned int m_id;
	
	// these should be references/pointers to avoid copies?
	// But buffers could also be reused, so we should just
	// free them when the vertexarray gets destroyed
	std::shared_ptr<GLIndexBuffer> m_indices;
	std::vector<std::shared_ptr<GLVertexBuffer>> m_buffers;
	unsigned int m_buffer_idx; // number of buffers

public:
	GLVertexArray();
	~GLVertexArray();

	void set(std::shared_ptr<GLIndexBuffer> indices);
	void push(std::shared_ptr<GLVertexBuffer> buffer);
	void bind() const;
	static void unbind() { glBindVertexArray(0); }
	uint32_t index_count() const { return m_indices->count(); }

	void update(size_t idx, void* data, size_t size) const;
	void set_layout(size_t idx, const GLBufferLayout& layout) const;
};