#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>
#include <cstring>

#include "../include/glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
  Shader() {}

  Shader(const std::string& vertex_shader_file_path,
         const std::string& fragment_shader_file_path) {
    unsigned int vertex_shader = compile(vertex_shader_file_path, GL_VERTEX_SHADER);
    unsigned int fragment_shader = compile(fragment_shader_file_path, GL_FRAGMENT_SHADER);

    program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader);
    glAttachShader(program_id, fragment_shader);
    glLinkProgram(program_id);

    // Link shaders
    int shader_link_success;
    char link_log[512];
    glGetProgramiv(program_id, GL_LINK_STATUS, &shader_link_success);
    if (!shader_link_success) {
      glGetProgramInfoLog(program_id, 512, NULL, link_log);
      std::ostringstream error_message;
      error_message << link_log;
      throw std::logic_error(error_message.str());
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
  }

  void bind() {
    glUseProgram(program_id);
  }

  void unbind() {
    glUseProgram(0);
  }

  void setFloat(const std::string& uniform_name, float value) {
    bind();
    int location = glGetUniformLocation(program_id, uniform_name.c_str());
    glUniform1f(location, value);
  }

  void setInt(const std::string& uniform_name, int value) {
    bind();
    int location = glGetUniformLocation(program_id, uniform_name.c_str());
    glUniform1i(location, value);
  }

  void setVec3(const std::string& uniform_name, glm::vec3 vec) {
    bind();
    int location = glGetUniformLocation(program_id, uniform_name.c_str());
    glUniform3f(location, vec.x, vec.y, vec.z);
  }

  void setVec2(const std::string& uniform_name, glm::vec2 vec) {
    bind();
    int location = glGetUniformLocation(program_id, uniform_name.c_str());
    glUniform2f(location, vec.x, vec.y);
  }

  void setMat4(const std::string& uniform_name, glm::mat4 value) {
    bind();
    int location = glGetUniformLocation(program_id, uniform_name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
  }

private:
  unsigned int program_id;

  unsigned int compile(const std::string& shader_file_path, unsigned int shader_type) {
    char* shader_source;
    std::ifstream shader_stream(shader_file_path, std::ios::in);
    if (shader_stream.is_open()) {
      std::stringstream shader_source_stream;
      shader_source_stream << shader_stream.rdbuf();
      std::string str = shader_source_stream.str();
      shader_source = new char[str.size() + 1];
      std::strcpy(shader_source, str.c_str());
      shader_stream.close();
    } else {
      std::stringstream error_message;
      error_message << "Shader at '" << shader_file_path << " was not found.";
      throw std::logic_error(error_message.str());
    }

    unsigned int shader;
    shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_source, NULL);
    glCompileShader(shader);

    int shader_compilation_success;
    char compilation_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_compilation_success);
    if (!shader_compilation_success) {
      glGetShaderInfoLog(shader, 512, NULL, compilation_log);
      std::ostringstream error_message;
      error_message << compilation_log << "\n"
                    << shader_file_path << "\n";
                    // "\n Shader source code: \n"
                    // << shader_source;
      throw std::logic_error(error_message.str());
    }

    return shader;
  }
};

#endif
