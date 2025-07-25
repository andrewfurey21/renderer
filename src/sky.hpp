#ifndef SKY_HPP
#define SKY_HPP

#include <vector>

#include "../include/glad/glad.h"

#include "camera.hpp"
#include "shader.hpp"
#include "helpers.hpp"

#ifndef STB_H
#define STB_H
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#endif

class Sky {
private:
  unsigned int id;
  unsigned int vbo_id;
  unsigned int vao_id;

  std::vector<std::string> file_paths;

  Shader shader;

public:
  Sky(std::string dir)
      : id(0) {
    file_paths = std::vector<std::string> {
      dir + "px.jpg",
      dir + "nx.jpg",
      dir + "py.jpg",
      dir + "ny.jpg",
      dir + "pz.jpg",
      dir + "nz.jpg",
    };

    shader = Shader(
      "../shaders/sky_vertex.glsl",
      "../shaders/sky_fragment.glsl"
    );

    shader.setInt("skyTexture", 0);
    std::vector<float> cubemap_vertices = {
      // ------ ------------
       -1.0f,  1.0f, -1.0f,
       -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
       -1.0f,  1.0f, -1.0f,
      // ------ ------------
       -1.0f, -1.0f,  1.0f,
       -1.0f, -1.0f, -1.0f,
       -1.0f,  1.0f, -1.0f,

       -1.0f,  1.0f, -1.0f,
       -1.0f,  1.0f,  1.0f,
       -1.0f, -1.0f,  1.0f,
      // ------ ------------
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,

        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
      // ------ ------------
       -1.0f, -1.0f,  1.0f,
       -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,

        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
       -1.0f, -1.0f,  1.0f,
      // ------ ------------
       -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,

        1.0f,  1.0f,  1.0f,
       -1.0f,  1.0f,  1.0f,
       -1.0f,  1.0f, -1.0f,
      // ------ ------------
       -1.0f, -1.0f, -1.0f,
       -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,

        1.0f, -1.0f, -1.0f,
       -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
      // ------ ------------
    };

    glGenVertexArrays(1, &vao_id);
    glGenBuffers(1, &vbo_id);
    glBindVertexArray(vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cubemap_vertices.size(), cubemap_vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);


    for (int i = 0; i < file_paths.size(); i++) {
      int width, height, channels;
      unsigned char* buffer = load_image(file_paths[i], &width, &height, &channels, false);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
                   0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
      stbi_image_free(buffer);
    }

    // settings for texture
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR); // how to resample down
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR); // how to resample up
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  }

  ~Sky() {}

  void draw(Camera &camera) {
    glDepthFunc(GL_LEQUAL);
    shader.bind();
    shader.setMat4("projection", camera.projection());
    shader.setMat4("view", glm::mat4(glm::mat3(camera.view())));

    glBindVertexArray(vao_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
  }
};

#endif
