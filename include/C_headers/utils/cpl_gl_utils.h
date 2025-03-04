#ifndef CPL_GL_UTILS_H
#define CPL_GL_UTILS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../CPLCore.h"
#include "shaders.h"

#define BUFFER_SIZE 512

void ShaderCheckError(GLuint object, GLenum statusType, const char* label,
                             void (*getiv)(GLuint, GLenum, GLint*),
                             void (*getInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*));

void SetUpShaders(GLuint* programID, GLuint* vbo, GLuint* vao, GLuint* vs, GLuint* fs);

void makeOrthoMatrix(float left, float right, float bottom, float top, float* out);

#endif // CPL_GL_UTILS_H