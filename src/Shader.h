#pragma once

#include <glad/glad.h>

#include <string>

class Shader {
public:
  Shader(const std::string& vertexPath, const std::string& fragmentPath);

  void use();

  GLuint getUniformLocation(const std::string &name) const;

private:
  GLuint m_id;
};
