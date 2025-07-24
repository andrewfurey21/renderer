#include <cmath>
#include <glm/ext/vector_float3.hpp>
#include <stdexcept>

#include "field.hpp"
#include "camera.hpp"
#include "input.hpp"
#include "shader.hpp"
#include "box.hpp"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

int main(void) {
  int width = 1600;
  int height = 1200;
  float near = 0.1f;
  float far = 100.0f;
  GLFWwindow* window = initialize_glfw(width, height);
  initialize_glad();
  setup_window(window, width, height);

  Camera camera(60.0f, (float)width, (float)height,
                  near, far, 0.05f, 0.15f);
  Input input(window);

  // ---------------------- Box -----------------------
  Shader box_shader(
    "../shaders/basic_box_vertex.glsl",
    "../shaders/basic_box_fragment.glsl"
  );

  std::vector<Box> boxes;
  for (size_t i = 0; i < 10; i++) {
    boxes.push_back(Box(box_shader));
    float y = 0;
    float x = 10 * cos(i);
    float z = 10 * sin(i);
    boxes[i].position(x, y, z);
    float r = static_cast<float>(rand()) / RAND_MAX;
    float g = static_cast<float>(rand()) / RAND_MAX;
    float b = static_cast<float>(rand()) / RAND_MAX;
    boxes[i].color(r, g, b);
  }
  // ----------------------------------------------------


  setup_imgui(window);
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    // ---------------------- INPUT -----------------------
    process_input(window);
    input.keyboard(camera);
    input.mouse(camera, width, height);
    // ----------------------------------------------------

    // ---------------------- Scene -----------------------
    for (Box box: boxes) {
      box.draw(camera);
    }
    // ----------------------------------------------------
    imgui_new_frame(window, width, height);

    glfwGetWindowSize(window, &width, &height);
    glfwSetWindowAspectRatio(window, width, height);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

GLFWwindow* initialize_glfw(int width, int height) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(width, height, "Renderer", NULL, NULL);
  glfwSetWindowAspectRatio(window, width, height);
  if (window == NULL) {
    glfwTerminate();
    throw std::logic_error("Failed to create window");
  }
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  return window;
}

void initialize_glad() {
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    glfwTerminate();
    throw std::logic_error("Failed to initialize glad.");
  }
}

void setup_window(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc(GL_LEQUAL);
}

void setup_imgui(GLFWwindow* window) {
  // ImGUI
  const char *glsl_version = "#version 330 core";
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  ImGui::StyleColorsDark();
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
}

void imgui_new_frame(GLFWwindow* window, int width, int height) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGuiIO& io = ImGui::GetIO();
  ImGui::NewFrame();
  ImGui::Begin("Editor");
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / io.Framerate, io.Framerate);
  ImGui::End();

  ImGui::Render();
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
