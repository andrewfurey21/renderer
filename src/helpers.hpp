#ifndef HELPERS_HPP
#define HELPERS_HPP
#include <string>
#include <sstream>

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

#ifdef DEBUG
#define GL_CHECK_ERROR() check_gl_error(__FILE__, __LINE__)
#else
#define GL_CHECK_ERROR()
#endif
#endif
