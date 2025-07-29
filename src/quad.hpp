#ifndef QUAD_HPP
#define QUAD_HPP

#include <vector>
#include "field.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "helpers.hpp"

class Quad {
public:
  Quad (Shader shader) : shader(shader) {
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
    glBindVertexArray(0);

    scale_vec = glm::vec3(1, 1, 1);
    position_vec = glm::vec3(0, 0, 0);
    color_vec = glm::vec3(1, 1, 1);

    radians = 0;
    rotation_axis = glm::vec3(0, 1, 0);

    use_texture = false;
  }

  void setTexture(std::string path) {
    glBindVertexArray(vao);
    use_texture = true;
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

    if (use_texture) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture_id);
    } else {
      shader.setVec3("color", color_vec);
    }

    glm::mat4 model(1.0f);
    model = glm::translate(model, position_vec);
    model = glm::rotate(model, radians, rotation_axis);
    model = glm::scale(model, scale_vec);
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  Shader shader;
private:
  unsigned int vao;
  unsigned int vbo;
  
  unsigned int texture_id;
  bool use_texture;

  glm::mat4 cameraProjection;

  glm::vec3 scale_vec;
  glm::vec3 position_vec;
  glm::vec3 color_vec;

  glm::vec3 rotation_axis;
  float radians;

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
