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

  // box
  float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
  // unsigned int indices[] = {
  //   0, 1, 2,
  //   0, 2, 3,
  // };

  // vertex array objects - specify multiple vertex attributes all in one place
  // ------------------ Box -------------------------
  unsigned int box_vao;
  glGenVertexArrays(1, &box_vao);
  glBindVertexArray(box_vao);

  unsigned int box_vbo; // for storing vertices
  glGenBuffers(1, &box_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, box_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy used defined data into the currently bound buffer
  // unsigned int ebo; // for storing indices
  // glGenBuffers(1, &ebo);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  // --------------------------------------------------
  // ---------------- Light source ---------------------
  unsigned int light_vao;
  glGenVertexArrays(1, &light_vao);
  glBindVertexArray(light_vao);
  
  unsigned int light_vbo;
  glGenBuffers(1, &light_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, light_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // --------------------------------------------------

  // ---------------- Texture ---------------------
  const std::string image_file_path =
    "/home/andrew/dev/graphics/renderer/assets/crate.png";
  int width, height, channels;
  unsigned char* image = load_image(image_file_path, &width,
    &height, &channels);

  unsigned int texture1;
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // how to resample down
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // how to resample up
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(image);



  const std::string other_file_path =
    // "/home/andrew/dev/graphics/renderer/assets/outer_crate.png";
    "/home/andrew/dev/graphics/renderer/assets/outer_crate.png";
  // int width, height, channels;
  unsigned char* other_image = load_image(other_file_path, &width,
    &height, &channels);

  unsigned int texture2;
  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // how to resample down
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // how to resample up
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, other_image);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(other_image);
  // ----------------------------------------------

  glm::vec3 light_color(1.0f, 1.0f, 1.0f);

  const std::string box_vertex_shader = "/home/andrew/dev/graphics/renderer/shaders/box_vertex.glsl";
  const std::string box_fragment_shader = "/home/andrew/dev/graphics/renderer/shaders/box_fragment.glsl";
  Shader box_shader(box_vertex_shader, box_fragment_shader);
  box_shader.setVec3("lightColor", light_color);
  // box_shader.setVec3("objectColor", glm::vec3(0.95f, 0.30f, 0.25f));
  box_shader.setVec3("objectColor", glm::vec3(1.0f));

  const std::string vertex_shader = "/home/andrew/dev/graphics/renderer/shaders/default_vertex.glsl";
  const std::string fragment_shader = "/home/andrew/dev/graphics/renderer/shaders/default_fragment.glsl";
  Shader default_shader(vertex_shader, fragment_shader);
  default_shader.setVec3("color", light_color);
  glm::vec3 light_position(0.0f, 2.0f, -9.0f);

  Camera camera(45.0f, (float)window_width, (float)window_height,
                  0.1f, 10.0f, 0.05f, 0.15f);
  Input input(window);
  bool debug_mode = false;
  box_shader.setMat4("projection", camera.projection());
  default_shader.setMat4("projection", camera.projection());

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
    process_input(window);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    if (debug_mode) {
      ImGui::Begin("Editor");
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / io.Framerate, io.Framerate);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // TODO: add toggle
      ImGui::End();
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // TODO: add toggle
    }

    input.keyboard(camera, &debug_mode);
    input.mouse(camera, window_width, window_height);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // glBindVertexArray(vao);

    // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_TRIANGLES, 0, 36);

    // ------------ Draw box ---------------------
    box_shader.bind();
    glBindVertexArray(box_vao);
    glm::mat4 model;
    model = glm::rotate(glm::mat4(1.0f), glm::radians(54.0f), light_position);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));

    box_shader.setMat4("view", camera.view());
    box_shader.setVec3("cameraPosition", camera.pos());
    box_shader.setMat4("model", model);
    // box_shader.setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
    box_shader.setInt("material.diffuse", 0);
    box_shader.setInt("material.specular", 1);
    // box_shader.setVec3("material.specular",glm::vec3( 0.5f, 0.5f, 0.5f));
    box_shader.setFloat("material.shininess", 32.0f);
    box_shader.setVec3("light.position", glm::vec3(0.0f, 2.0f, -9.0f));
    box_shader.setVec3("light.ambient",  glm::vec3(0.2f, 0.2f, 0.2f));
    box_shader.setVec3("light.diffuse",  glm::vec3(0.5f, 0.5f, 0.5f));
    box_shader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // ----------------------------------------

    // ------------ Draw light---------------------
    default_shader.bind();
    glBindVertexArray(light_vao);
    model = glm::rotate(glm::mat4(1.0f), glm::radians(10.0f), glm::vec3(-1.5f, 3.0f, -9.0f));
    model = glm::translate(model, light_position);

    default_shader.setMat4("view", camera.view());
    default_shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // ----------------------------------------

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
