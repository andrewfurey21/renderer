#ifndef FIELD_HPP
#define FIELD_HPP

#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
GLFWwindow* initialize_glfw(int width, int height);
void initialize_glad();
void setup_window(GLFWwindow* window, int width, int height);

#endif


