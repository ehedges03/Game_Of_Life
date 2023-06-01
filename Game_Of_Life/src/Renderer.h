#pragma once

#include <GL/glew.h>

//#include "ArrayBuffer.h"
//#include "IndexBuffer.h"

#define ASSERT(x) if (!(x)) __debugbreak();
// TODO: Dev version attaches this, but prod version just has passthrough, not sure how to do this
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

// Helpful documentaion: https://docs.gl/

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

//class Renderer {
//public:
//	//static void Draw(const VertexArray & va, const IndexBuffer & ib, const Shader & shader) const;
//}
