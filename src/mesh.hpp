#ifndef MESH_HPP
#define MESH_HPP

#include "../include/glad/glad.h"

#include <vector>

#include <glm/glm.hpp>

#include "shader.hpp"

#define MAX_BONE_WEIGHTS 4

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 textureCoords;

  // animation related.
  int boneIds[MAX_BONE_WEIGHTS];
  float weights[MAX_BONE_WEIGHTS];
};

struct Texture {
  unsigned int id;
  std::string type;
  std::string path;
};

class Mesh {
public:
  Mesh(const std::vector<Vertex>& vertices,
       const std::vector<unsigned int>& indices,
       const std::vector<Texture> textures) :
    vertices(vertices),
    indices(indices),
    textures(textures) {

    // TODO: addTexture instead.

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(Vertex) * vertices.size(),
                 vertices.data(),
                 GL_STATIC_DRAW);


    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*)(offsetof(Vertex, position)));

    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*)(offsetof(Vertex, normal)));

    // texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*)(offsetof(Vertex, textureCoords)));

    // boneIds
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3,
                          MAX_BONE_WEIGHTS,
                          GL_INT,
                          sizeof(Vertex),
                          (void*)(offsetof(Vertex, boneIds)));
    // weights
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4,
                          MAX_BONE_WEIGHTS,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*)(offsetof(Vertex, weights)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(unsigned int) * indices.size(),
                 indices.data(),
                 GL_STATIC_DRAW);

    glBindVertexArray(0);
  }

  // returns number of draw calls made
  int draw(Shader& shader) {
    unsigned int diffuse = 1;
    unsigned int specular = 1;
    for(unsigned int i = 0; i < textures.size(); i++) {
      glActiveTexture(GL_TEXTURE0 + i);
      std::string number;
      std::string name = textures[i].type;
      if(name == "texture_diffuse") { 
        number = std::to_string(diffuse++);
      }
      else if(name == "texture_specular") {
        number = std::to_string(specular++);
      }
      else {
        std::ostringstream error_message;
        error_message << "Texture type not found: "
                      << name << ".";
        throw std::logic_error(error_message.str());
      }

      shader.setInt((name + number).c_str(), i);
      // shader.setInt(("material." + name + number).c_str(), i);
      glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    return 1;
  }

private:
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  unsigned int vao, vbo, ebo;

};

#endif
