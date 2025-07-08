#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

unsigned int shader_compiler(const std::string& shader_file_path, unsigned int shader_type) {
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
                  << shader_file_path << "\n"
                  "\n Shader source code: \n"
                  << shader_source;
    throw std::logic_error(error_message.str());
  }

  return shader;
}

int main(void) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  size_t windowWidth = 800;
  size_t windowHeight = 600;

  GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Renderer", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create window.\n";
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD.\n";
    glfwTerminate();
    return -1;
  }

  glViewport(0, 0, windowWidth, windowHeight);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // Vertex data and vbo
  float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
  };

  // vertex array objects - specify multiple vertex attributes all in one place
  unsigned int vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  unsigned int vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy used defined data into the currently bound buffer
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);  

  // compile shaders
  unsigned int vertex_shader = shader_compiler("/home/andrew/dev/graphics/renderer/src/vertex.glsl", GL_VERTEX_SHADER);
  unsigned int fragment_shader = shader_compiler("/home/andrew/dev/graphics/renderer/src/fragment.glsl", GL_FRAGMENT_SHADER);

  unsigned int shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  // Link shaders
  int shader_link_success;
  char link_log[512];
  glGetProgramiv(shader_program, GL_LINK_STATUS, &shader_link_success);
  if (!shader_link_success) {
    glGetProgramInfoLog(shader_program, 512, NULL, link_log);
    std::ostringstream error_message;
    error_message << link_log;
    throw std::logic_error(error_message.str());
  }

  glUseProgram(shader_program);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  while (!glfwWindowShouldClose(window)) {
    process_input(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
