#pragma once

#include <array>
#include <vector>

#include <glad/gl.h>

#include "GLBufferLayout.hpp"


class GLVertexBuffer {
private:
	unsigned int m_id;
	GLBufferLayout m_layout;
	size_t m_size;
	unsigned int m_mode;

public:
	
	GLVertexBuffer(size_t bytesize, unsigned int mode = GL_STREAM_DRAW);
	GLVertexBuffer(void* vertices, size_t bytesize, unsigned int mode = GL_STREAM_DRAW);
	template <typename T, size_t N>
	GLVertexBuffer(std::array<T, N>& vertices, unsigned int mode = GL_STREAM_DRAW)
		: GLVertexBuffer(vertices.data(), N * sizeof(T), mode)
	{}
	template <typename T>
	GLVertexBuffer(std::vector<T>& vertices, unsigned int mode = GL_STREAM_DRAW)
		: GLVertexBuffer(vertices.data(), vertices.size() * sizeof(T), mode)
	{}

	~GLVertexBuffer();

	void set_data(const void* vertices, unsigned int size);
	void set_layout(const GLBufferLayout& layout);

	void bind() const;
	void unbind() const;
	uint32_t size() const { return m_size; }

	GLBufferLayout get_layout() const;
};


class GLIndexBuffer {
private:
	unsigned int m_id;
	size_t m_size = 0;
	unsigned int m_mode;

public:
	
	GLIndexBuffer(size_t size, unsigned int mode = GL_STATIC_DRAW);
	GLIndexBuffer(uint32_t* indices, size_t size, unsigned int mode = GL_STATIC_DRAW);
	template <size_t N>
	GLIndexBuffer(std::array<uint32_t, N>& vertices, unsigned int mode = GL_STREAM_DRAW)
		: GLIndexBuffer(vertices.data(), N, mode)
	{}
	GLIndexBuffer(std::vector<uint32_t>& vertices, unsigned int mode = GL_STREAM_DRAW) 
		: GLIndexBuffer(vertices.data(), vertices.size(), mode) 
	{}

	~GLIndexBuffer();

	void set(const uint32_t * indices, size_t size);
	void bind() const;
	void unbind() const;
	uint32_t size() const { return m_size; }
};


class GLVertexArray
{
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
	uint32_t index_count() const { return m_indices->size(); }

	void update(size_t idx, void* data, size_t size) const;
	void set_layout(size_t idx, const GLBufferLayout& layout) const;
};


