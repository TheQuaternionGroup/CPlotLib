#ifndef CPL_UTILS_H
#define CPL_UTILS_H

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define BUFFER_SIZE 512

//-------------------------------------
// Internal data
//-------------------------------------

static GLuint s_curveVAO    = 0;
static GLuint s_curveVBO    = 0;
static GLsizei s_curveCount = 0;

static GLuint s_axisVAO   = 0;
static GLuint s_axisVBO   = 0;
static GLsizei s_axisCount = 0;

// Auto-detected domain
static float s_minX = -1.f, s_maxX = 1.f;
static float s_minY = -1.f, s_maxY = 1.f;

static int   s_isDataLoaded = 0;


//------------------------------------------------------------------------------
// Helper funcs for compiling/linking
//------------------------------------------------------------------------------

char* ReadFile(const char* filename);

void CheckError(GLuint object, GLenum statusType, const char* label, void (*getiv)(GLuint, GLenum, GLint*), void (*getInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*));

GLuint CompileShader(const char* src, GLenum type);

GLuint LinkProgram(GLuint vs, GLuint fs);

void makeOrthoMatrix(float left, float right, float bottom, float top, float* out);

void generateVAOandVBO(GLuint* vao, GLuint* vbo);

#endif // CPL_UTILS_H