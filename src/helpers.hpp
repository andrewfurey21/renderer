#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <string>
#include <stb/stb_image.h>
#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>

void check_gl_error(const char* file, int line);
#ifdef DEBUG
#define GL_CHECK_ERROR() check_gl_error(__FILE__, __LINE__)
#else
#define GL_CHECK_ERROR()
#endif

unsigned char* load_image(const std::string& file_path, int* width, int* height, int* channels, bool flip);

#endif
