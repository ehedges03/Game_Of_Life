#include "BitArray.h"
#include "Utils/WrappedPoint.h"
#include "GameBoard.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

void simpleBitArrayTest();
void simpleWrappedPointTest();

// Helpful documentaion: https://docs.gl/

static std::string readFile(const std::string& filepath) {
    std::ifstream stream(filepath);

    std::stringstream sstr;

    while (stream >> sstr.rdbuf());

    return sstr.str();
}

static uint32_t CompileShader(uint32_t type, const std::string& source) {
    uint32_t id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = new char[length];
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!" << std::endl;
        std::cout << message << std::endl;
    }

    return id;
}

static uint32_t CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    uint32_t program = glCreateProgram();
    uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

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

    std::string vertexShader = readFile("res/shaders/Basic.vertex.glsl");

    std::string fragmentShader = readFile("res/shaders/Basic.fragment.glsl");

    uint32_t shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

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

    glDeleteProgram(shader);

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