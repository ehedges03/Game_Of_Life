#include "BitArray.h"
#include "Utils/WrappedPoint.h"
#include "GameBoard.h"
//#include <libFunni/log.h>
//#include <libFunni/pointtech.h>
#include <iostream>
#include "Renderer.h"
#include <GLFW/glfw3.h>

#include "ArrayBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

void simpleBitArrayTest();
void simpleWrappedPointTest();
//void simpleLoggerTest();

// Helpful documentaion: https://docs.gl/

int main()
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit()) {
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windows mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cout << "ERROR!\n";
	}

	GLCall(std::cout << glGetString(GL_VERSION) << std::endl);
	{
		float basicPositions[] = {
		     0.025f,  -0.975f,
			 0.975f, -0.975f,
			 0.975f, -0.025f,
			 0.025f, -0.025f,
		};

		float fullPositions[64];

		int idx = 0, offset = 0;
		for (int i = 0; i < 8; i++) {
			offset = i;
			for (int j = 0; j < 8; j++) {
				fullPositions[idx] = basicPositions[j] + (j % 2 == 0 ? offset : 0);
				idx++;
			}
		}

		uint32_t basicIndices[] = {
			0, 1, 2,
			2, 3, 0,
		};

		uint32_t fullIndices[48];

		idx = 0;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 6; j++) {
				fullIndices[idx] = basicIndices[j] + (i * 4);
				idx++;
			}
		}

		uint32_t vao;
		GLCall(glGenVertexArrays(1, &vao));
		GLCall(glBindVertexArray(vao));

		ArrayBuffer vb(fullPositions, 4 * 2 * 8 * sizeof(float), true);

		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));

		IndexBuffer ib(fullIndices, 6 * 8);

		glm::mat4 proj = glm::ortho(0.0f, 12.0f, 0.0f, -9.0f, -1.0f, 1.0f);

		Shader shader("res/shaders/Basic.vertex.glsl", "res/shaders/Basic.fragment.glsl");
		shader.Bind();

		shader.SetUniformMat4f("u_MVP", proj);
		shader.SetUniformUint("u_state", 0b01010101);

		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window)) {
			/* Render here */
			GLCall(glClear(GL_COLOR_BUFFER_BIT));

			GLCall(glBindVertexArray(vao));
			ib.Bind();
			shader.Bind();

			GLCall(glDrawElementsInstanced(GL_TRIANGLES, 6 * 8, GL_UNSIGNED_INT, nullptr, 8));

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}

	glfwTerminate();

	simpleBitArrayTest();
	simpleWrappedPointTest();
	//simpleLoggerTest();
	return 0;
}

void simpleBitArrayTest() {
	BitArray mybits = BitArray(9);

	std::cout << mybits << '\n';

	mybits.set(0, true);
	mybits.set(2, true);
	mybits.set(4, true);
	mybits.set(6, true);
	mybits.set(8, true);

	std::cout << mybits << '\n';

	mybits.set(0, false);
	mybits.set(4, false);
	mybits.set(8, false);

	std::cout << mybits << '\n';
}

void simpleWrappedPointTest() {
	// test WrappedPoint -- imagine all the unit tests yay

	WrappedPoint testWPoint1({ -2, -6 }, { 5, 5 });
	WrappedPoint testWPoint2({ 5, 15 }, { 5, 5 });

	bool result1 = (testWPoint1.x() == 3) && (testWPoint1.y() == 4);
	bool result2 = (testWPoint2.x() == 0) && (testWPoint2.y() == 0);

    std::cout << '\n' << "negative wrap: " << (result1 ? "success " : "failed ") << testWPoint1.x() << ", " << testWPoint1.y();
    std::cout << '\n' << "positive wrap: " << (result2 ? "success " : "failed ") << testWPoint2.x() << ", " << testWPoint2.y();
}

//void simpleloggertest() {
//
//    // only logd will function
//    funni::Logger<true, false, false, false> logger("logtest");
//
//    std::cout << "\n\n";
//
//    logger.Start();
//
//    logger.logd();
//    logger.logi();
//    logger.logw();
//    logger.logi();
//
//}