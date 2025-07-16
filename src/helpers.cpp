#include <string>
#include <sstream>

#include <stb/stb_image.h>

#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>


void check_gl_error(const char* file, int line) {
  GLenum err;
  while((err = glGetError()) != GL_NO_ERROR) {
    std::string errorType;
    switch (err) {
      case GL_INVALID_ENUM:                  errorType = "invalid enum"; break;
      case GL_INVALID_VALUE:                 errorType = "invalid value"; break;
      case GL_INVALID_OPERATION:             errorType = "invalid operation"; break;
      case GL_OUT_OF_MEMORY:                 errorType = "out of memory"; break;
      case GL_INVALID_FRAMEBUFFER_OPERATION: errorType = "invalid framebuffer operation"; break;
      default: errorType = "Unknown"; break;
    }
    std::ostringstream error_message;
    error_message << "OpenGL Error:\n"
                  << err << "(" << errorType << ") at " << file << ":" << line;
    throw std::logic_error(error_message.str());
  }
}

unsigned char* load_image(const std::string& file_path, int* width, int* height, int* channels, bool flip) {
  stbi_set_flip_vertically_on_load(flip);
  unsigned char* image = stbi_load(file_path.c_str(), width, height, channels, 0);
  if (!image) {
    std::ostringstream error_message;
    error_message << "Could not open image at: "
                  << file_path;
    throw std::logic_error(error_message.str());
  }
  return image;
}

#ifdef DEBUG
#define GL_CHECK_ERROR() check_gl_error(__FILE__, __LINE__)
#else
#define GL_CHECK_ERROR()
#endif
