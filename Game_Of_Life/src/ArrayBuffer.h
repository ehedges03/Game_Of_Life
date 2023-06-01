#pragma once
#include <stdint.h>

class ArrayBuffer {
public:
	ArrayBuffer(const void* data, uint32_t size, bool isStatic);
	~ArrayBuffer();

	void Bind() const;
	void Unbind() const;

private:
	uint32_t m_RendererID;
};

