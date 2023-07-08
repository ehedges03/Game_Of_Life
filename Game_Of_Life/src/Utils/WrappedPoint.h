#pragma once

#include <stdint.h>
#include <utility>
#include <type_traits>
#include <array>

/*

Utility for easily creating an object representing an integer defined point in 2D space
and restricting it to the bounds of a wrapping array/bitmap/what-have-you represented with only positive integers

*/

class WrappedPoint {

public:
	// Branchless: first statement reduces index magnitude to less than the bound
	// second statement handles sign if there is one
	// note: bounds are length not max index
	WrappedPoint(std::array<int, 2> rpoint, const uint32_t(&bound)[2]) {
		m_bounds[0] = bound[0];
		m_bounds[1] = bound[1];
		m_x = rpoint[0] % (int)bound[0];
		m_y = rpoint[1] % (int)bound[1];
		m_y = (rpoint[1] + bound[1]) % (int)bound[1];
	}

	~WrappedPoint() = default;

	uint32_t x() { return (m_x + m_bounds[0]) % (int)m_bounds[0]; }
	uint32_t x(int offset) { return (m_x + offset + m_bounds[0]) % (int)m_bounds[0]; }
	uint32_t y() { return (m_y + m_bounds[1]) % (int)m_bounds[1]; }
	uint32_t y(int offset) { return (m_y + offset + m_bounds[1]) % (int)m_bounds[1]; }

	uint32_t getOffset(int xOffset, int yOffset) {
		return x(xOffset) + y(yOffset) * m_bounds[1];
	}

private:
	uint32_t m_bounds[2];
	uint32_t m_x;
	uint32_t m_y;

};