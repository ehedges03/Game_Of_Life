#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
	Shader(const std::string& vertexFileName, const std::string& fragmentFileName);
	~Shader();

	void Bind() const;
	void Unbind() const;

	// Set uniforms
	void SetUniform4f(const std::string& name, const glm::vec4 value);
	void SetUniformMat4f(const std::string& name, const glm::mat4 matrix);
	void SetUniformUint(const std::string& name, const uint32_t value);

private:
	uint32_t m_RendererID;

	bool CompileShader();
	uint32_t GetUniformLocation(const std::string& name);
};
