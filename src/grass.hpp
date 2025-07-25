#ifndef GRASS_HPP
#define GRASS_HPP

#include <vector>
#include "field.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "helpers.hpp"

class Grass {
public:
  Grass(Shader shader, size_t num, float ground_y) : num_instances(num), shader(shader) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    float grass_height = 2.0f;
    float grass_y = ground_y + 1.45f;
    float x_range = sqrt((double)num_instances);
    float z_range = sqrt((double)num_instances);
    glm::vec3 grass_axis(0, 1, 0);
  
    for (size_t i = 0; i < num_instances; i++) {
      glm::mat4 model(1.0f);
      float angle = (static_cast<float>(rand()) / RAND_MAX) * 90.0f;
      float x = (static_cast<float>(rand()) / RAND_MAX) * x_range - x_range / 2.0f;
      float z = (static_cast<float>(rand()) / RAND_MAX) * z_range - z_range / 2.0f;
      model = glm::translate(model, glm::vec3(x, grass_y, z));
      model = glm::rotate(model, glm::radians(angle), grass_axis);
      model = glm::scale(model, glm::vec3(1, 2.5, 1));
      models.push_back(model);
    }

    glGenBuffers(1, &model_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model_vbo);
    glBufferData(GL_ARRAY_BUFFER, num_instances * sizeof(glm::mat4), models.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(1 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  void setTexture(std::string path) {
    glBindVertexArray(vao);
    int width, height, channels;
    unsigned char* image = load_image(path, &width, &height, &channels, true);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // how to resample down
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // how to resample up
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(image);
    shader.setInt("inputTexture", 0);
    glBindVertexArray(0);
  }

  void position(float r, float g, float b) {
    position_vec = glm::vec3(r, g, b);
  }

  void scale(float x, float y, float z) {
    scale_vec = glm::vec3(x, y, z);
  }

  void color(float r, float g, float b) {
    if (r > 1.0f || r < 0.0f || g > 1.0f || g < 0.0f || b > 1.0f || b < 0.0f) {
      std::ostringstream errorMessage;
      errorMessage << "Color must be between 0 and 1, you gave " << r << ", " << g << ", " << b;
      throw std::logic_error(errorMessage.str());
    }
    color_vec = glm::vec3(r, g, b);
  }

  void rotation(float radians, glm::vec3 axis) {
    rotation_axis = axis;
    this->radians = radians;
  }

  void draw(Camera& camera) {
    shader.bind();
    glBindVertexArray(vao);

    if (camera.projection() != cameraProjection) {
      cameraProjection = camera.projection();
      shader.setMat4("projection", camera.projection());
    }
    shader.setMat4("view", camera.view());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, num_instances);
  }

private:
  unsigned int vao;
  unsigned int vbo;
  unsigned int model_vbo;
  
  unsigned int texture_id;
  unsigned int num_instances;

  glm::mat4 cameraProjection;

  glm::vec3 scale_vec;
  glm::vec3 position_vec;
  glm::vec3 color_vec;

  glm::vec3 rotation_axis;
  float radians;

  std::vector<glm::mat4> models;

  Shader shader;
  std::vector<float> vertices = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, 0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, 0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, 0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,

         0.5f,  0.5f, 0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, 0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, 0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
  };

};

#endif
