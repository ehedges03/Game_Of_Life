#include "Shader.h"
#include "Renderer.h"
#include <iostream>
#include <fstream>
#include <sstream>

static std::string readFile(const std::string& filepath);
static uint32_t CompileShader(uint32_t type, const std::string& source);
static uint32_t CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

Shader::Shader(const std::string& vertexFilePath, const std::string& fragmentFilePath): m_RendererID(0) {
	std::string vertexShader = readFile(vertexFilePath);
	std::string fragmentShader = readFile(fragmentFilePath);

	m_RendererID = CreateShader(vertexShader, fragmentShader);
}

Shader::~Shader() {
	GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const {
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const {
	GLCall(glUseProgram(m_RendererID));
}

void Shader::SetUniform4f(const std::string& name, const glm::vec4 value) {
	GLCall(glUniform4f(GetUniformLocation(name), value[0], value[1], value[2], value[3]));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4 matrix) {
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

void Shader::SetUniformUint(const std::string& name, const uint32_t value) {
	GLCall(glUniform1ui(GetUniformLocation(name), value));
}

uint32_t Shader::GetUniformLocation(const std::string& name) {
	GLCall(uint32_t location = glGetUniformLocation(m_RendererID, name.c_str()));
	return location;
}

static std::string readFile(const std::string& filepath) {
	std::ifstream stream(filepath);

	std::stringstream sstr;

	while (stream >> sstr.rdbuf());

	return sstr.str();
}

static uint32_t CompileShader(uint32_t type, const std::string& source) {
	GLCall(uint32_t id = glCreateShader(type));
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE) {
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = new char[length];
		GLCall(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!" << std::endl;
		std::cout << message << std::endl;
	}

	return id;
}

static uint32_t CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	GLCall(uint32_t program = glCreateProgram());
	GLCall(uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader));
	GLCall(uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader));

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}

