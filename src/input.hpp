#ifndef INPUT_HPP
#define INPUT_HPP

#include "camera.hpp"
#include <GLFW/glfw3.h>

class Input {
private:
  GLFWwindow *window;

public:
  Input(GLFWwindow *window) : window(window) {}
  ~Input() {}

  void keyboard(Camera &camera) {
    static bool key_pressed = false;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      camera.forward();
    } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      camera.backward();
    } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      camera.left();
    } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      camera.right();
    } else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
      camera.up();
    } else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
      camera.down();
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      key_pressed = true;

    } else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
      if (key_pressed) {
        key_pressed = false;
      }
    }
  }

  void mouse(Camera &camera, int width, int height) {
    double xpos, ypos;
    double new_xpos, new_ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    camera.mouse_look(glm::vec2(xpos, ypos));
  }
};

#endif
