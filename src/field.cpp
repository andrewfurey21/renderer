#include <cmath>
#include <glm/ext/vector_float3.hpp>
#include <stdexcept>

#include "field.hpp"
#include "camera.hpp"
#include "grass.hpp"
#include "input.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "box.hpp"
#include "sky.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

int main(void) {
  srand(time(NULL));
  int width = 1600;
  int height = 1200;
  float near = 0.1f;
  float far = 10000.0f;
  GLFWwindow* window = initialize_glfw(width, height);
  initialize_glad();
  setup_window(window, width, height);

  // ---------------------- Camera + Input --------------
  Camera camera(60.0f, (float)width, (float)height,
                  near, far, 1.0f, 0.15f);
  camera.set_pos(0, 3, 0);
  Input input(window);

  // ---------------------- Sky -----------------------
  Sky night_sky("../assets/stars/");
  // ----------------------------------------------------

  // ---------------------- Boxes -----------------------
  size_t num_boxes = 10;
  std::vector<Box> boxes;
  for (size_t i = 0; i < num_boxes; i++) {
    Shader box_shader(
      "../shaders/basic_box_vertex.glsl",
      "../shaders/basic_box_fragment.glsl"
    );
    boxes.push_back(Box(box_shader));
    float y = 0;
    float x = 10 * cos(glm::radians((float)i * 360.0f / num_boxes));
    float z = 10 * sin(glm::radians((float)i * 360.0f / num_boxes));
    boxes[i].position(x, y, z);
    float r = static_cast<float>(rand()) / RAND_MAX;
    float g = static_cast<float>(rand()) / RAND_MAX;
    float b = static_cast<float>(rand()) / RAND_MAX;
    boxes[i].color(r, g, b);
  }
  // ----------------------------------------------------

  // ---------------------- ground -----------------------
  Shader ground_shader(
    "../shaders/basic_box_vertex.glsl",
    "../shaders/basic_box_fragment.glsl"
  );
  Box ground(ground_shader);
  float ground_y = -3.0f;
  ground.scale(1000, 1, 1000);
  ground.color(0.03f, 0.1f, 0.07f);

  // ---------------------- grass -----------------------

  size_t num_grass = 1000000;

  Shader grass_shader(
    "../shaders/grass_vertex.glsl",
    "../shaders/grass_fragment.glsl"
  );
  Grass grass(grass_shader, num_grass, ground_y);
  grass.setTexture("../assets/grass_cut.png");
  // ---------------------- moonlight ------------------

  Shader moon_shader(
    "../shaders/basic_box_vertex.glsl",
    "../shaders/basic_box_fragment.glsl"
  );
  Box moon(moon_shader);
  moon.color(1.0f, 1.0f, 1.0f);
  moon.position(336, 321, 83);
  // -0.71511, -0.624562, -0.313911
  // ---------------------- tree -----------------------
  size_t num_trees = 30;
  std::vector<Model> trees;
  for (size_t i = 0; i < num_trees; i++) {
    float x_range = num_trees * 10;
    float z_range = num_trees * 10;
    float x = (static_cast<float>(rand()) / RAND_MAX) * x_range - x_range / 2.0f;
    float z = (static_cast<float>(rand()) / RAND_MAX) * z_range - z_range / 2.0f;
    // float angle = (static_cast<float>(rand()) / RAND_MAX) * 90.0f;
    Shader tree_shader(
      "../shaders/tree_model_vertex.glsl",
      "../shaders/tree_model_fragment.glsl"
    );
    Model tree_model(tree_shader, "../assets/tree/oak_tree.obj");
    tree_model.set_scale(.05, .09, .05);
    tree_model.set_angle(270, glm::vec3(1, 0, 0));
    tree_model.set_pos(x, ground_y, z);

    // Moon Light
    tree_model.shader.setVec3("light.direction", glm::vec3(-0.71511, -0.624562, -0.313911));
    tree_model.shader.setVec3("light.ambient",  glm::vec3(0.2f, 0.2f, 0.2f));
    tree_model.shader.setVec3("light.diffuse",  glm::vec3(0.5f, 0.5f, 0.5f));
    tree_model.shader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    tree_model.shader.setVec3("light.color", glm::vec3(1.0f, 1.0f, 1.0f));

    // cameraPosition, light direction, ambient, diffuse, specular, color

    trees.push_back(tree_model);
  }

  // ---------------------- misc -----------------------

  setup_imgui(window);

  // ---------------------- RENDER LOOP -----------------------
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    // ---------------------- INPUT -----------------------
    process_input(window);
    input.keyboard(camera);
    input.mouse(camera, width, height);
    // ----------------------------------------------------

    // ---------------------- Scene -----------------------
    night_sky.draw(camera);

    glm::vec3 camera_pos = camera.pos();
    ground.position(camera_pos.x, ground_y, camera_pos.z);
    ground.draw(camera);
    for (Box box: boxes) {
      box.draw(camera);
    }

    grass.draw(camera);

    for (Model tree: trees)
      tree.draw(camera);

    moon.draw(camera);
    // ----------------------------------------------------

    imgui_new_frame(window, width, height, camera);
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

static bool CURSOR = true;

void process_input(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  } else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
    CURSOR = !CURSOR;
    if (CURSOR)
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

void imgui_new_frame(GLFWwindow* window, int width, int height, Camera& camera) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGuiIO& io = ImGui::GetIO();
  ImGui::NewFrame();
  ImGui::Begin("Debug");
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / io.Framerate, io.Framerate);
  glm::vec3 pos = camera.pos();
  ImGui::Text("Camera: %.3f x, %.3f y, %.3f z", pos.x, pos.y, pos.z);
  ImGui::End();

  ImGui::Render();
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
