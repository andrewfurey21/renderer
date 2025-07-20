#include <cstring>
#include <iostream>
#include <cmath>

#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// #include "model.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "input.hpp"
#include "helpers.hpp"
#include "sky.hpp"


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}


int main(void) {
  Assimp::Importer importer;
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  int window_width = 800;
  int window_height = 600;

  GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Renderer", NULL, NULL);
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

  glViewport(0, 0, window_width, window_height);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc(GL_LEQUAL);

  // Camera camera(45.0f, (float)window_width, (float)window_height,
  //                 0.1f, 10.0f, 0.05f, 0.15f);
  // Input input(window);
  bool debug_mode = true;

  // ImGUI
  const char *glsl_version = "#version 330 core";
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  ImGui::StyleColorsDark();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  float quad_vertices[] = {
    // positions     // colors
    -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
     0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
    -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

    -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
     0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
     0.05f,  0.05f,  0.0f, 1.0f, 1.0f,
  };

  glm::vec2 translations[100];
  int index = 0;
  float offset = 0.1f;
  for(int y = -10; y < 10; y += 2) {
    for(int x = -10; x < 10; x += 2) {
      glm::vec2 translation;
      translation.x = (float)x / 10.0f + offset;
      translation.y = (float)y / 10.0f + offset;
      translations[index++] = translation;
    }
  }

  unsigned int vao;
  unsigned int vbo;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
  glBindVertexArray(0);

  Shader instance_shader("/home/andrew/dev/graphics/renderer/shaders/instancing_vertex.glsl",
			 "/home/andrew/dev/graphics/renderer/shaders/instancing_fragment.glsl");
  instance_shader.bind();
  for (int i=  0; i < 100; i++) {
    std::stringstream offset;
    offset << "offsets[" << std::to_string(i) << "]";
    instance_shader.setVec2(offset.str(), translations[i]);
  }

  while (!glfwWindowShouldClose(window)) {
    process_input(window);
    // input.keyboard(camera, &debug_mode);
    // input.mouse(camera, window_width, window_height);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    instance_shader.bind();
    glBindVertexArray(vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);


    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (debug_mode) {
      ImGui::Begin("Editor");
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / io.Framerate, io.Framerate);
      // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // TODO: add toggle
      ImGui::End();
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // TODO: add toggle
    }

    ImGui::Render();
    glfwGetFramebufferSize(window, &window_width, &window_height);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
