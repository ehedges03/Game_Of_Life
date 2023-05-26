#pragma once

#include <GL/glew.h>

#define ASSERT(x) if (!(x)) __debugbreak();
// TODO: Dev version attaches this, but prod version just has passthrough, not sure how to do this
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

// Helpful documentaion: https://docs.gl/

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);


