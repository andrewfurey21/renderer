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

// #include "../imgui/imgui.h"
// #include "../imgui/imgui_impl_glfw.h"
// #include "../imgui/imgui_impl_opengl3.h"

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

  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_FRONT);
  // glFrontFace(GL_CW);

  // box
  // float vertices[] = {
  //       // positions          // normals           // texture coords
  //       -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
  //        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
  //        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
  //        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
  //       -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
  //       -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
  //
  //       -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
  //        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
  //        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
  //        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
  //       -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
  //       -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
  //
  //       -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
  //       -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
  //       -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
  //       -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
  //       -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
  //       -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
  //
  //        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
  //        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
  //        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
  //        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
  //        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
  //        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
  //
  //       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
  //        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
  //        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
  //        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
  //       -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
  //       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
  //
  //       -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
  //        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
  //        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
  //        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
  //       -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
  //       -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
  //   };
  // // unsigned int indices[] = {
  // //   0, 1, 2,
  // //   0, 2, 3,
  // // };
  //
  // glm::vec3 cubePositions[] = {
  //   glm::vec3( 0.0f,  0.0f,  0.0f),
  //   glm::vec3( 2.0f,  5.0f, -15.0f),
  //   glm::vec3(-1.5f, -2.2f, -2.5f),
  //   glm::vec3(-3.8f, -2.0f, -12.3f),
  //   glm::vec3( 2.4f, -0.4f, -3.5f),
  //   glm::vec3(-1.7f,  3.0f, -7.5f),
  //   glm::vec3( 1.3f, -2.0f, -2.5f),
  //   glm::vec3( 1.5f,  2.0f, -2.5f),
  //   glm::vec3( 1.5f,  0.2f, -1.5f),
  //   glm::vec3(-1.3f,  1.0f, -1.5f)
  // };
  //
  // glm::vec3 pointLightPositions[] = {
	 //  glm::vec3( 0.7f,  0.2f,  2.0f),
	 //  glm::vec3( 2.3f, -3.3f, -4.0f),
	 //  glm::vec3(-4.0f,  2.0f, -12.0f),
	 //  glm::vec3( 0.0f,  0.0f, -3.0f)
  // };
  //
  // glm::vec3 pointLightColors[] = {
	 //  glm::vec3( 1.0f,  0.0f,  0.0f),
	 //  glm::vec3( 0.0f, 1.0f, 0.0f),
	 //  glm::vec3(0.0f,  0.0f, 1.0f),
	 //  glm::vec3( 1.0f,  1.0f, 1.0f)
  // };
  //
  // float quadVertices[] = {
  //       // positions   // texture coords
  //       -1.0f,  1.0f,  0.0f, 1.0f,
  //       -1.0f, -1.0f,  0.0f, 0.0f,
  //        1.0f, -1.0f,  1.0f, 0.0f,
  //
  //       -1.0f,  1.0f,  0.0f, 1.0f,
  //        1.0f, -1.0f,  1.0f, 0.0f,
  //        1.0f,  1.0f,  1.0f, 1.0f
  // };
  //
  // glEnable(GL_DEPTH_TEST);
  //
  // // ------------------ Screen -------------------------
  //
  // unsigned int quadVAO, quadVBO;
  // glGenVertexArrays(1, &quadVAO);
  // glGenBuffers(1, &quadVBO);
  // glBindVertexArray(quadVAO);
  // glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
  // glEnableVertexAttribArray(0);
  // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  // glEnableVertexAttribArray(1);
  // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  // glBindVertexArray(0);
  // // -------------------------------------------
  //
  // // vertex array objects - specify multiple vertex attributes all in one place
  // // ------------------ Box -------------------------
  // unsigned int box_vao;
  // glGenVertexArrays(1, &box_vao);
  // glBindVertexArray(box_vao);
  //
  // unsigned int box_vbo; // for storing vertices
  // glGenBuffers(1, &box_vbo);
  // glBindBuffer(GL_ARRAY_BUFFER, box_vbo);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy used defined data into the currently bound buffer
  // // unsigned int ebo; // for storing indices
  // // glGenBuffers(1, &ebo);
  // // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  // // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  //
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  // glEnableVertexAttribArray(0);
  // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  // glEnableVertexAttribArray(1);
  // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  // glEnableVertexAttribArray(2);
  // glBindVertexArray(0);
  // --------------------------------------------------
  // ---------------- Light source ---------------------
  // unsigned int light_vao;
  // glGenVertexArrays(1, &light_vao);
  // glBindVertexArray(light_vao);
  // 
  // unsigned int light_vbo;
  // glGenBuffers(1, &light_vbo);
  // glBindBuffer(GL_ARRAY_BUFFER, light_vbo);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  //
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  // glEnableVertexAttribArray(0);
  // --------------------------------------------------
  // ----------- Frame buffer ------------
  // unsigned int fbo;
  // glGenFramebuffers(1, &fbo);
  // glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  //
  // unsigned int colorbuffer;
  // glGenTextures(1, &colorbuffer);
  // glBindTexture(GL_TEXTURE_2D, colorbuffer);
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glBindTexture(GL_TEXTURE_2D, 0);
  //
  // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorbuffer, 0);
  //
  // unsigned int rbo;
  // glGenRenderbuffers(1, &rbo);
  // glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height);
  // glBindRenderbuffer(GL_RENDERBUFFER, 0);
  //
  // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
  //
  // if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  //   throw std::logic_error("Framebuffer is not complete.");
  // glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // glDeleteFramebuffers(1, &fbo);
  // -------------------------------------------

  // ---------------- Texture ---------------------
  // const std::string image_file_path =
  //   "/home/andrew/dev/graphics/renderer/assets/crate.png";
  // int width, height, channels;
  // unsigned char* image = load_image(image_file_path, &width,
  //   &height, &channels, true);
  //
  // unsigned int texture1;
  // glGenTextures(1, &texture1);
  // glBindTexture(GL_TEXTURE_2D, texture1);
  //
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // how to resample down
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // how to resample up
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  //
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
  // glGenerateMipmap(GL_TEXTURE_2D);
  // glBindTexture(GL_TEXTURE_2D, 0);
  // stbi_image_free(image);

  // const std::string other_file_path =
  //   "/home/andrew/dev/graphics/renderer/assets/outer_crate.png";
  // // int width, height, channels;
  // unsigned char* other_image = load_image(other_file_path, &width,
  //   &height, &channels);
  //
  // unsigned int texture2;
  // glGenTextures(1, &texture2);
  // glBindTexture(GL_TEXTURE_2D, texture2);
  //
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // how to resample down
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // how to resample up
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  //
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, other_image);
  // glGenerateMipmap(GL_TEXTURE_2D);
  // glBindTexture(GL_TEXTURE_2D, 0);
  // stbi_image_free(other_image);
  // ----------------------------------------------

  // glm::vec3 light_color(1.0f, 1.0f, 1.0f);

  // const std::string box_vertex_shader = "/home/andrew/dev/graphics/renderer/shaders/basic_texture_vertex.glsl";
  // const std::string box_fragment_shader = "/home/andrew/dev/graphics/renderer/shaders/basic_texture_fragment.glsl";
  // Shader box_shader(box_vertex_shader, box_fragment_shader);
  // box_shader.setVec3("lightColor", light_color);
  // box_shader.setVec3("objectColor", glm::vec3(0.95f, 0.30f, 0.25f));
  // box_shader.setVec3("objectColor", glm::vec3(1.0f));

  // const std::string vertex_shader = "/home/andrew/dev/graphics/renderer/shaders/default_vertex.glsl";
  // const std::string fragment_shader = "/home/andrew/dev/graphics/renderer/shaders/default_fragment.glsl";
  // Shader default_shader(vertex_shader, fragment_shader);
  // default_shader.setVec3("color", light_color);
  // glm::vec3 light_position(0.0f, 2.0f, -9.0f);

  Camera camera(45.0f, (float)window_width, (float)window_height,
                  0.1f, 10.0f, 0.05f, 0.15f);
  // Model bag_model("/home/andrew/dev/graphics/renderer/assets/backpack/backpack.obj");
  // Shader bag_model_shader("/home/andrew/dev/graphics/renderer/shaders/basic_vertex.glsl",
  //                     "/home/andrew/dev/graphics/renderer/shaders/basic_model_fragment.glsl");
  Input input(window);
  bool debug_mode = true;

  // box_shader.setMat4("projection", camera.projection());
  // box_shader.setInt("inputTexture", 0);
  // screen_shader.setInt("screenTexture", 0);

  // ImGUI
  // const char *glsl_version = "#version 330 core";
  // IMGUI_CHECKVERSION();
  // ImGui::CreateContext();
  // ImGui_ImplGlfw_InitForOpenGL(window, true);cmake
  // ImGui_ImplOpenGL3_Init(glsl_version);
  // ImGui::StyleColorsDark();
  // ImGuiIO &io = ImGui::GetIO();
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  std::vector<std::string> sky_paths {
    "/home/andrew/dev/graphics/renderer/assets/stars/px.jpg", // right
    "/home/andrew/dev/graphics/renderer/assets/stars/nx.jpg", // left
    "/home/andrew/dev/graphics/renderer/assets/stars/py.jpg", // top
    "/home/andrew/dev/graphics/renderer/assets/stars/ny.jpg", // bottom
    "/home/andrew/dev/graphics/renderer/assets/stars/pz.jpg", // front
    "/home/andrew/dev/graphics/renderer/assets/stars/nz.jpg", // back
    // "/home/andrew/dev/graphics/renderer/assets/night/right.jpg",
    // "/home/andrew/dev/graphics/renderer/assets/night/left.jpg",
    // "/home/andrew/dev/graphics/renderer/assets/night/top.jpg",
    // "/home/andrew/dev/graphics/renderer/assets/night/bottom.jpg",
    // "/home/andrew/dev/graphics/renderer/assets/night/front.jpg",
    // "/home/andrew/dev/graphics/renderer/assets/night/back.jpg",
    // "/home/andrew/dev/graphics/renderer/assets/day/right.jpg",
    // "/home/andrew/dev/graphics/renderer/assets/day/left.jpg",
    // "/home/andrew/dev/graphics/renderer/assets/day/top.jpg",
    // "/home/andrew/dev/graphics/renderer/assets/day/bottom.jpg",
    // "/home/andrew/dev/graphics/renderer/assets/day/front.jpg",
    // "/home/andrew/dev/graphics/renderer/assets/day/back.jpg",
  };

  Sky sky(sky_paths);

  while (!glfwWindowShouldClose(window)) {
    process_input(window);
    input.keyboard(camera, &debug_mode);
    input.mouse(camera, window_width, window_height);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    sky.draw(camera);

    // glBindVertexArray(vao);

    // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_TRIANGLES, 0, 36);

    // ------------ Draw box ---------------------

    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, texture2);

    // box_shader.setVec3("cameraPosition", camera.pos());

    // Box material
    // box_shader.setInt("material.diffuse", 0);
    // box_shader.setInt("material.specular", 1);
    // box_shader.setFloat("material.shininess", 32.0f);

    // // Directional Light
    // box_shader.setVec3("sun.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    // box_shader.setVec3("sun.color", glm::vec3(0.95f, 0.56f, 0.18f));
    // box_shader.setFloat("sun.ambient",  0.2f);
    // box_shader.setFloat("sun.diffuse",  0.5f);
    // box_shader.setFloat("sun.specular", 1.0f);
    //
    // // point light 1
    // box_shader.setVec3("lights[0].position", pointLightPositions[0]);
    // box_shader.setVec3("lights[0].color", pointLightColors[0]);
    // box_shader.setFloat("lights[0].ambient", 0.05f);
    // box_shader.setFloat("lights[0].diffuse", 0.8f);
    // box_shader.setFloat("lights[0].specular", 1.0f);
    // box_shader.setFloat("lights[0].constant", 1.0f);
    // box_shader.setFloat("lights[0].linear", 0.09f);
    // box_shader.setFloat("lights[0].quadratic", 0.032f);
    // // point light 2
    // box_shader.setVec3("lights[1].position", pointLightPositions[1]);
    // box_shader.setVec3("lights[1].color", pointLightColors[1]);
    // box_shader.setFloat("lights[1].ambient", 0.05f);
    // box_shader.setFloat("lights[1].diffuse", 0.8f);
    // box_shader.setFloat("lights[1].specular", 1.0f);
    // box_shader.setFloat("lights[1].constant", 1.0f);
    // box_shader.setFloat("lights[1].linear", 0.09f);
    // box_shader.setFloat("lights[1].quadratic", 0.032f);
    // // point light 3
    // box_shader.setVec3("lights[2].position", pointLightPositions[2]);
    // box_shader.setVec3("lights[2].color", pointLightColors[2]);
    // box_shader.setFloat("lights[2].ambient", 0.05f);
    // box_shader.setFloat("lights[2].diffuse", 0.8f);
    // box_shader.setFloat("lights[2].specular", 1.0f);
    // box_shader.setFloat("lights[2].constant", 1.0f);
    // box_shader.setFloat("lights[2].linear", 0.09f);
    // box_shader.setFloat("lights[2].quadratic", 0.032f);
    // // point light 4
    // box_shader.setVec3("lights[3].position", pointLightPositions[3]);
    // box_shader.setVec3("lights[3].color", pointLightColors[3]);
    // box_shader.setFloat("lights[3].ambient", 0.05f);
    // box_shader.setFloat("lights[3].diffuse", 0.8f);
    // box_shader.setFloat("lights[3].specular", 1.0f);
    // box_shader.setFloat("lights[3].constant", 1.0f);
    // box_shader.setFloat("lights[3].linear", 0.09f);
    // box_shader.setFloat("lights[3].quadratic", 0.032f);
    //
    // box_shader.setVec3("torch.position",  camera.pos());
    // box_shader.setVec3("torch.direction", camera.view_dir());
    // box_shader.setVec3("torch.color", glm::vec3(1.0f, 1.0f, 1.0f));
    // box_shader.setFloat("torch.cutOff",   glm::cos(glm::radians(12.5f)));
    // box_shader.setFloat("torch.outerCutOff",   glm::cos(glm::radians(15.0f)));
    // box_shader.setFloat("torch.constant", 1.0f);
    // box_shader.setFloat("torch.linear", 0.45f);
    // box_shader.setFloat("torch.quadratic", 0.16f);



    // for(unsigned int i = 0; i < 10; i++) {
      // glm::mat4 model(1.0f);
      // model = glm::translate(model, cubePositions[i]);
      // float angle = 20.0f * i;
      // model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      // box_shader.setMat4("model", model);
      //
      // glDrawArrays(GL_TRIANGLES, 0, 36);
    // }

    // glDrawArrays(GL_TRIANGLES, 0, 36);
    // ----------------------------------------
    // ------------ Draw light---------------------
    // default_shader.bind();
    // glBindVertexArray(light_vao);
    //
    // for (int i = 0; i < 4; i++) {
    //   glm::mat4 model;
    //   model = glm::translate(model, pointLightPositions[i]);
    //
    //   default_shader.setVec3("color", pointLightColors[i]);
    //   default_shader.setMat4("view", camera.view());
    //   default_shader.setMat4("model", model);
    //   glDrawArrays(GL_TRIANGLES, 0, 36);
    // }
    // ----------------------------------------
    
    // glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // glEnable(GL_DEPTH_TEST);
    // glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    // glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // box_shader.bind();
    // glm::mat4 model(1.0f);
    // model = glm::translate(model, glm::vec3(0, 0, 0));
    // box_shader.setMat4("model", model);
    // box_shader.setMat4("view", camera.view());
    // glBindVertexArray(box_vao);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, texture1);
    // glDrawArrays(GL_TRIANGLES, 0, 36);

    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glDisable(GL_DEPTH_TEST);
    // //
    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT);
    // //
    // screen_shader.bind();
    // glBindVertexArray(quadVAO);
    // glBindTexture(GL_TEXTURE_2D, colorbuffer);
    // glDrawArrays(GL_TRIANGLES, 0, 6);

    // ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplGlfw_NewFrame();
    // ImGui::NewFrame();
    //
    // if (debug_mode) {
    //   ImGui::Begin("Editor");
    //   ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
    //               1000.0f / io.Framerate, io.Framerate);
    //   // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // TODO: add toggle
    //   ImGui::End();
    // } else {
    //   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // TODO: add toggle
    // }
    //
    // ImGui::Render();
    glfwGetFramebufferSize(window, &window_width, &window_height);
    // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
