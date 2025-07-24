#include <stdexcept>

#include "field.hpp"
#include "camera.hpp"
#include "input.hpp"

int main(void) {
  int width = 800;
  int height = 600;
  GLFWwindow* window = initialize_glfw(width, height);
  initialize_glad();
  setup_window(window, width, height);

  Camera camera(45.0f, (float)width, (float)height,
                  0.1f, 10.0f, 0.05f, 0.15f);
  Input input(window);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    // ---------------------- INPUT -----------------------
    process_input(window);
    input.keyboard(camera);
    input.mouse(camera, width, height);
    // ----------------------------------------------------

    glfwGetFramebufferSize(window, &width, &height);
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
