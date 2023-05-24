#include "BitArray.h"
#include "Utils/WrappedPoint.h"
#include "GameBoard.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void simpleBitArrayTest();
void simpleWrappedPointTest();

int main()
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

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

    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    uint32_t buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();

    simpleBitArrayTest();
    simpleWrappedPointTest();
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