#include <cstring>
#include <iostream>
#include <cmath>

#include <stb/stb_image.h>

#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>

#include "shader.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

unsigned char* load_image(const std::string& file_path, int* width, int* height, int* channels) {
  stbi_set_flip_vertically_on_load(1);
  unsigned char* image = stbi_load(file_path.c_str(), width, height, channels, 0);
  if (!image) {
    std::ostringstream error_message;
    error_message << "Could not open image at: "
                  << file_path;
    throw std::logic_error(error_message.str());
  }
  return image;
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

  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc(GL_LEQUAL);

  // Loading texture
  const std::string coin_file_path =
    "/home/andrew/dev/graphics/renderer/assets/coin.png";
  int coin_width, coin_height, coin_channels;
  unsigned char* coin_image = load_image(coin_file_path, &coin_width, 
    &coin_height, &coin_channels);

  const std::string emoji_file_path =
    "/home/andrew/dev/graphics/renderer/assets/emoji.png";
  int emoji_width, emoji_height, emoji_channels;
  unsigned char* emoji_image = load_image(emoji_file_path, &emoji_width, 
    &emoji_height, &emoji_channels);

  // Vertex data and vbo
  float vertices[] = {
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,
  };
  unsigned int indices[] = {
    0, 1, 2,
    0, 2, 3,
  };

  // vertex array objects - specify multiple vertex attributes all in one place
  unsigned int vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  unsigned int vbo; // for storing vertices
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy used defined data into the currently bound buffer
  unsigned int ebo; // for storing indices
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  unsigned int texture1;
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // how to resample down
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // how to resample up
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, coin_width, coin_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, coin_image);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(coin_image);

  unsigned int texture2;
  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // how to resample down
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // how to resample up
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, emoji_width, emoji_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, emoji_image);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(emoji_image);

  const std::string vertex_shader = "/home/andrew/dev/graphics/renderer/src/vertex.glsl";
  const std::string fragment_shader = "/home/andrew/dev/graphics/renderer/src/fragment.glsl";
  Shader shader(vertex_shader, fragment_shader);

  shader.use();
  shader.setInt("texture1", 0);
  shader.setInt("texture2", 1);

  while (!glfwWindowShouldClose(window)) {
    process_input(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    shader.use();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);


    glBindVertexArray(vao);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // TODO: add toggle
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
