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

#include "model.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "input.hpp"


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

  Camera camera(45.0f, (float)window_width, (float)window_height,
                  0.1f, 10.0f, 0.05f, 0.15f);
  Model bag_model("/home/andrew/dev/graphics/renderer/assets/backpack/backpack.obj");
  Shader bag_model_shader("/home/andrew/dev/graphics/renderer/shaders/basic_vertex.glsl",
                      "/home/andrew/dev/graphics/renderer/shaders/basic_model_fragment.glsl");
  Input input(window);
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

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    process_input(window);
    input.keyboard(camera, &debug_mode);
    input.mouse(camera, window_width, window_height);

    // ------------ Draw model ---------------------
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

    bag_model_shader.setMat4("projection", camera.projection());
    bag_model_shader.setMat4("view", camera.view());
    bag_model_shader.setMat4("model", model);
    bag_model.draw(bag_model_shader);
    // ----------------------------------------

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
    glViewport(0, 0, window_width, window_height);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
