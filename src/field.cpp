#include <cmath>
#include <glm/ext/vector_float3.hpp>
#include <stdexcept>

#include "field.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

const float ground_y = -3.0f;
unsigned int quadVAO = 0;
unsigned int quadVBO;

int main(void) {
  srand(time(NULL));
  int width = 1600;
  int height = 1200;
  float near = 0.1f;
  float far = 1000.0f;
  GLFWwindow *window = initialize_glfw(width, height);
  initialize_glad();
  setup_window(window, width, height);

  // ---------------------- Camera + Input --------------
  glm::vec3 light_pos(168., 160.5, 41.5);
  glm::vec3 light_dir(-0.71511, -0.624562, -0.313911);

  glm::mat4 light_projection =
      glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near, far);
  glm::mat4 light_view =
      glm::lookAt(light_pos,
                  light_pos + light_dir,
                  glm::vec3(0.0f, 1.0f, 0.0f));

  Camera camera(60.0f, (float)width, (float)height, near, far, 0.5f, 0.15f);
  // camera.set_pos(168, 160.5, 41.5);
  // camera.view_direction = light_dir;
  Input input(window);

  Shader depth_shader("../shaders/depth_shader_vertex.glsl",
                       "../shaders/depth_shader_fragment.glsl");
  depth_shader.setMat4("projection", light_projection);
  depth_shader.setMat4("view", light_view);

  // Shader quad_debug_shader(
  //   "../shaders/debug_quad_depth_vertex.glsl",
  //   "../shaders/debug_quad_depth_fragment.glsl"
  // );
  //
  // quad_debug_shader.setInt("depthMap", 0);

  // ---------------------- Sky -----------------------
  Sky night_sky("../assets/stars/");
  // ----------------------------------------------------

  // ---------------------- ground -----------------------
  Shader ground_shader(
    "../shaders/ground_vertex.glsl",
    "../shaders/ground_fragment.glsl"
  );
  Box ground(ground_shader);
  ground.scale(100000, 1, 100000);
  // ground.color(0.03f, 0.1f, 0.07f);
  ground.color(0.1f, 0.9f, 0.35f);
  ground.shadow_shader = depth_shader;
  ground.shader.setInt("shadowMap", 0);
  set_directional_light(ground.shader);
  ground.shader.setMat4("light_proj", light_projection);
  ground.shader.setMat4("light_view", light_view);
  // ---------------------- grass -----------------------

  size_t num_grass = 1000000;

  Shader grass_shader(
    "../shaders/grass_vertex.glsl",
    "../shaders/grass_fragment.glsl"
  );
  Grass grass(grass_shader, num_grass, ground_y);
  grass.setTexture("../assets/grass_cut.png");
  set_directional_light(grass.shader);
  // ---------------------- tree -----------------------
  size_t num_trees = 1;
  std::vector<Model> trees;
  for (size_t i = 0; i < num_trees; i++) {
    float x_range = num_trees * 100;
    float z_range = num_trees * 100;
    // float x = (static_cast<float>(rand()) / RAND_MAX) * x_range - x_range
    // / 2.0f; float z = (static_cast<float>(rand()) / RAND_MAX) * z_range -
    // z_range / 2.0f;
    float x = 0;
    float z = 0;
    Shader tree_shader("../shaders/tree_model_vertex.glsl",
                       "../shaders/tree_model_fragment.glsl");
    Model tree_model(tree_shader, "../assets/tree/oak_tree.obj");
    tree_model.set_scale(.05, .09, .05);
    tree_model.set_angle(270, glm::vec3(1, 0, 0));
    tree_model.set_pos(x, ground_y, z);

    // Moon Light
    set_directional_light(tree_model.shader);

    tree_model.shadow_shader = depth_shader;

    trees.push_back(tree_model);
  }
  // ---------------------- character -------------------
  Shader character_shader(
    "../shaders/character_vertex.glsl",
    "../shaders/character_fragment.glsl"
  );

  const std::string character_file_path =
  "../assets/vampire/dancing_vampire.dae"; Model character(character_shader,
  character_file_path); Animation character_animation(character_file_path,
  &character); Animator character_animator(&character_animation);
  character.set_scale(2, 2, 2);
  character.set_pos(50, -1, 1);

  set_directional_light(character.shader);
  // ---------------------- shadow framebuffers --------
  unsigned int depth_fbo;
  glGenFramebuffers(1, &depth_fbo);

  const unsigned shadow_width = 1024;
  const unsigned shadow_height = 1024;

  unsigned int depth_map;
  glGenTextures(1, &depth_map);
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width,
               shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depth_map, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // ---------------------- misc -----------------------

  setup_imgui(window);
  float deltaTime = 0;
  float lastFrame = 0;

  // ---------------------- RENDER LOOP -----------------------
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    // ---------------------- INPUT -----------------------
    process_input(window);
    input.keyboard(camera);
    input.mouse(camera, width, height);

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // character_animator.UpdateAnimation(deltaTime);
    // ---------------------- Scene -----------------------

    // render to the depth map
    glViewport(0, 0, shadow_width, shadow_height);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    render_shadows(camera, trees, ground, light_view);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render (including shadow mapping)
    glViewport(0, 0, width, height);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // quad_debug_shader.bind();
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, depth_map);
    // render_quad();

    render_scene(camera, night_sky, ground, trees, grass, character, character_animator, depth_map);

    // ----------------------------------------------------

    imgui_new_frame(window, width, height, camera, deltaTime);
    glfwGetWindowSize(window, &width, &height);
    glfwSetWindowAspectRatio(window, width, height);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void set_directional_light(Shader &shader) {
  shader.setVec3("light.direction", glm::vec3(-0.71511, -0.624562, -0.313911));
  shader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
  shader.setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
  shader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
  shader.setVec3("light.color", glm::vec3(1.0f, 1.0f, 1.0f));
}

void render_shadows(Camera &camera, std::vector<Model> &trees, Box& ground, glm::mat4& light_view) {
  // ground.shadow_shader.setMat4("view", light_view);
  // ground.draw(camera, true);
  for (Model tree : trees) {
    // tree.shadow_shader.setMat4("view", light_view);
    tree.draw(camera, true);
  }
}

void render_scene(Camera &camera, Sky &night_sky, Box &ground,
                  std::vector<Model> &trees, Grass &grass, Model &character,
                  Animator &animator, unsigned int depth_map) {
  night_sky.draw(camera);

  glm::vec3 camera_pos = camera.pos();
  ground.position(camera_pos.x, ground_y, camera_pos.z);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depth_map);
  ground.draw(camera);

  // grass.shader.setFloat("time", glfwGetTime());
  // grass.draw(camera);

  for (Model tree : trees)
    tree.draw(camera);

  // std::vector<glm::mat4> &transforms = animator.GetFinalBoneMatrices();
  // for (int i = 0; i < transforms.size(); ++i) {
  //   character.shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]",
  //                            transforms[i]);
  // }
  // character.shader.setVec3("cameraPosition", camera.pos());
  // character.draw(camera);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

static bool CURSOR = true;

void process_input(GLFWwindow *window) {
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

GLFWwindow *initialize_glfw(int width, int height) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(width, height, "Renderer", NULL, NULL);
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

void setup_window(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc(GL_LEQUAL);
}

void setup_imgui(GLFWwindow *window) {
  // ImGUI
  const char *glsl_version = "#version 330 core";
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  ImGui::StyleColorsDark();
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
}

void imgui_new_frame(GLFWwindow *window, int width, int height, Camera &camera,
                     float deltaTime) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGuiIO &io = ImGui::GetIO();
  ImGui::NewFrame();
  ImGui::Begin("Debug");
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / io.Framerate, io.Framerate);
  glm::vec3 pos = camera.pos();
  ImGui::Text("Camera: %.3f x, %.3f y, %.3f z", pos.x, pos.y, pos.z);
  ImGui::Text("Delta Time: %.3f", deltaTime);
  ImGui::End();

  ImGui::Render();
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void render_quad() {
  if (quadVAO == 0) {
    float quadVertices[] = {
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
      1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
  }
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void print_mat4(const glm::mat4& m) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      std::cout << m[i][j];
      if (j == 3)
        std::cout << "\n";
      else std::cout << ", ";
    }
  }
}
