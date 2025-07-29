#ifndef FIELD_HPP
#define FIELD_HPP

#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>

#include "camera.hpp"
#include "grass.hpp"
#include "input.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "box.hpp"
#include "sky.hpp"
#include "animation.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
GLFWwindow* initialize_glfw(int width, int height);
void initialize_glad();
void setup_window(GLFWwindow* window, int width, int height);
void setup_imgui(GLFWwindow* window);
void imgui_new_frame(GLFWwindow* window, int width, int height, Camera& camera, float deltaTime);
void print_mat4(const glm::mat4& m);
void render_scene(Camera& camera, Sky& night_sky, Box& ground, std::vector<Model>& trees, Grass& grass, Model& character, Animator& animator, unsigned int depth_map);
void render_shadows(Camera &camera, std::vector<Model> &trees, Box& ground, glm::mat4& light_view);
void set_directional_light(Shader& shader);
void render_quad();

#endif


