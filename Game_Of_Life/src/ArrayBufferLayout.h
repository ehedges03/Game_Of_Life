#pragma once
#include <vector>
#include "Renderer.h"

struct ArrayBufferElement {
	uint32_t type;
	uint32_t count;
	bool normalized;

	static uint32_t GetSizeOfType(uint32_t type) {
		switch (type) {
			case GL_FLOAT: return sizeof(GL_FLOAT);
			case GL_UNSIGNED_BYTE: return sizeof(GL_UNSIGNED_BYTE);
		};

		ASSERT(false);
		return 0;
	}
};

class ArrayBufferLayout {
public:
	ArrayBufferLayout() : m_Stride(0) {};

	template<typename t>
	void Push(uint32_t count) {
		static_assert(false);
	}

	template<>
	void Push<float>(uint32_t count) {
		m_Elements.push_back({GL_FLOAT, count, false});
		m_Stride += count * ArrayBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<uint8_t>(uint32_t count) {
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, false });
		m_Stride += count * ArrayBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	inline uint32_t GetStride() const { return m_Stride; };

private:
	std::vector<ArrayBufferElement> m_Elements;
	uint32_t m_Stride;
};
