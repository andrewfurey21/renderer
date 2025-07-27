#ifndef MODEL_HPP
#define MODEL_HPP

#include <glm/ext/vector_float3.hpp>
#include <map>
#include <stb/stb_image.h>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "camera.hpp"
#include "mesh.hpp"
#include "shader.hpp"

struct BoneInfo {
  int id;
  glm::mat4 offset;
};

class Model {
public:
  Model(Shader shader, const std::string &file_path) : shader(shader) {
    Assimp::Importer importer;
    const aiScene *scene =
        // importer.ReadFile(file_path, aiProcess_Triangulate | aiProcess_FlipUVs);
        importer.ReadFile(file_path, aiProcess_Triangulate);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
      std::cout << "scene pointer: " << scene << "\n";
      std::ostringstream error_message;
      error_message << "Could not import model at: " << file_path << "\n" << importer.GetErrorString();
      throw std::logic_error(error_message.str());
    }
    dir = file_path.substr(0, file_path.find_last_of("/"));
    processNode(scene->mRootNode, scene);

    angle = 0;
    scale = glm::vec3(1, 1, 1);
    position = glm::vec3(0, 0, 0);
  }

  int draw(Camera &camera) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);
    model = glm::rotate(model, angle, axis);
    shader.setMat4("projection", camera.projection());
    shader.setMat4("view", camera.view());
    shader.setMat4("model", model);
    int draw_calls = 0;
    for (int i = 0; i < meshes.size(); i++)
      draw_calls += meshes[i].draw(shader);
    return draw_calls;
  }

  void set_pos(float x, float y, float z) { position = glm::vec3(x, y, z); }

  void set_scale(float x, float y, float z) { scale = glm::vec3(x, y, z); }

  void set_angle(float angle, glm::vec3 axis) {
    this->angle = glm::radians(angle);
    this->axis = axis;
  }

  Shader shader;

  std::map<std::string, BoneInfo> &get_bone_info_map() {
    return bone_info_map;
  }
  int bone_count() { return bone_counter; }

private:
  glm::vec3 position;
  glm::vec3 scale;
  float angle;
  glm::vec3 axis;

  std::vector<Mesh> meshes;
  std::string dir;
  std::vector<Texture> textures_loaded;

  std::map<std::string, BoneInfo> bone_info_map;
  int bone_counter;


  void SetVertexBoneDataToDefault(Vertex &vertex) {
    for (int i = 0; i < MAX_BONE_WEIGHTS; i++) {
      vertex.boneIds[i] = -1;
      vertex.weights[i] = 0.0f;
    };
  }

  void processNode(aiNode *node, const aiScene *scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
      meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
      processNode(node->mChildren[i], scene);
    }
  }

  unsigned int TextureFromFile(const char *path, const std::string &directory,
                               bool gamma = false) {
    std::string filename(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(false);
    unsigned char *data =
        stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
      GLenum format;
      if (nrComponents == 1)
        format = GL_RED;
      else if (nrComponents == 3)
        format = GL_RGB;
      else if (nrComponents == 4)
        format = GL_RGBA;

      glBindTexture(GL_TEXTURE_2D, textureID);
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                      GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      stbi_image_free(data);
    } else {
      std::cout << "Texture failed to load at path: " << path << std::endl;
      stbi_image_free(data);
    }

    return textureID;
  }

  std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            std::string typeName) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
      aiString str;
      mat->GetTexture(type, i, &str);
      bool skip = false;
      for (unsigned int j = 0; j < textures_loaded.size(); j++) {
        if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
          textures.push_back(textures_loaded[j]);
          skip = true;
          break;
        }
      }
      if (!skip) { // if texture hasn't been loaded already, load it
        Texture texture;
        texture.id = TextureFromFile(str.C_Str(), dir);
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back(texture);
        textures_loaded.push_back(texture); // add to loaded textures
      }
    }
    return textures;
  }

  Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
    // lots of copying here...
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      Vertex vertex;
      glm::vec3 vector;
      vector.x = mesh->mVertices[i].x;
      vector.y = mesh->mVertices[i].y;
      vector.z = mesh->mVertices[i].z;
      vertex.position = vector;

      vector.x = mesh->mNormals[i].x;
      vector.y = mesh->mNormals[i].y;
      vector.z = mesh->mNormals[i].z;
      vertex.normal = vector;

      if (mesh->mTextureCoords[0]) {
        glm::vec2 vec;
        vec.x = mesh->mTextureCoords[0][i].x;
        vec.y = mesh->mTextureCoords[0][i].y;
        vertex.textureCoords = vec;
      } else {
        vertex.textureCoords = glm::vec2(0.0f, 0.0f);
      }

      vertices.push_back(vertex);
    }
    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
      aiFace face = mesh->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; j++)
        indices.push_back(face.mIndices[j]);
    }
    // process material
    if (mesh->mMaterialIndex >= 0) {
      if (mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(
            material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(
            material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(),
                        specularMaps.end());
      }
    }

    ExtractBoneWeightForVertices(vertices, mesh, scene);

    return Mesh(vertices, indices, textures);
  }

  void SetVertexBoneData(Vertex &vertex, int boneID, float weight) {
    for (int i = 0; i < MAX_BONE_WEIGHTS; ++i) {
      if (vertex.boneIds[i] < 0) {
        vertex.weights[i] = weight;
        vertex.boneIds[i] = boneID;
        break;
      }
    }
  }

  glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4 &from) {
    glm::mat4 to;
    to[0][0] = from.a1;
    to[1][0] = from.a2;
    to[2][0] = from.a3;
    to[3][0] = from.a4;
    to[0][1] = from.b1;
    to[1][1] = from.b2;
    to[2][1] = from.b3;
    to[3][1] = from.b4;
    to[0][2] = from.c1;
    to[1][2] = from.c2;
    to[2][2] = from.c3;
    to[3][2] = from.c4;
    to[0][3] = from.d1;
    to[1][3] = from.d2;
    to[2][3] = from.d3;
    to[3][3] = from.d4;
    return to;
  }

  void ExtractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh *mesh,
                                    const aiScene *scene) {
    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
      int boneID = -1;
      std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
      if (bone_info_map.find(boneName) == bone_info_map.end()) {
        BoneInfo newBoneInfo;
        newBoneInfo.id = bone_counter;
        newBoneInfo.offset = ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
        bone_info_map[boneName] = newBoneInfo;
        boneID = bone_counter;
        bone_counter++;
      } else {
        boneID = bone_info_map[boneName].id;
      }
      assert(boneID != -1);
      auto weights = mesh->mBones[boneIndex]->mWeights;
      int numWeights = mesh->mBones[boneIndex]->mNumWeights;

      for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
        int vertexId = weights[weightIndex].mVertexId;
        float weight = weights[weightIndex].mWeight;
        assert(vertexId <= vertices.size());
        SetVertexBoneData(vertices[vertexId], boneID, weight);
      }
    }
  }
};

#endif
