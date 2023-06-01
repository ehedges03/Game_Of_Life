#include "ArrayBuffer.h"

#include "Renderer.h"

ArrayBuffer::ArrayBuffer(const void* data, uint32_t size, bool isStatic) {
	GLCall(glGenBuffers(1, &m_RendererID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, isStatic ? GL_STATIC_DRAW : GL_STREAM_DRAW));
}

ArrayBuffer::~ArrayBuffer() {
	GLCall(glDeleteBuffers(1, &m_RendererID));
}

void ArrayBuffer::Bind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void ArrayBuffer::Unbind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
