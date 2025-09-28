#ifndef CPL_UTILS_H
#define CPL_UTILS_H

#include <GL/glew.h>
#include <math.h>
#include "CPLPlot.h"

// Matrix utilities
void cpl_make_ortho_matrix(float left, float right, float bottom, float top, float* out);

// Plot rendering (declared in CPLPlot.h)

// Error handling
void cpl_error(const char* message);
void cpl_warning(const char* message);

// Math utilities
float cpl_clamp(float value, float min, float max);
float cpl_lerp(float a, float b, float t);

#endif // CPL_UTILS_H
