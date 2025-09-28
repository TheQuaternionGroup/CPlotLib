#include "CPLUtils.h"
#include "CPLPlot.h"
#include <stdio.h>

void cpl_make_ortho_matrix(float left, float right, float bottom, float top, float* out) {
    if (!out) return;
    
    float z_near = -1.0f;
    float z_far = 1.0f;
    
    // Avoid division by zero
    if (fabsf(right - left) < 1e-14f) right = left + 1.0f;
    if (fabsf(top - bottom) < 1e-14f) top = bottom + 1.0f;
    
    // Initialize to identity
    for (int i = 0; i < 16; i++) {
        out[i] = 0.0f;
    }
    
    // Set orthographic projection matrix
    out[0] = 2.0f / (right - left);
    out[5] = 2.0f / (top - bottom);
    out[10] = -2.0f / (z_far - z_near);
    out[12] = -(right + left) / (right - left);
    out[13] = -(top + bottom) / (top - bottom);
    out[14] = -(z_far + z_near) / (z_far - z_near);
    out[15] = 1.0f;
}

// cpl_render_plot is implemented in CPLPlot.c

void cpl_error(const char* message) {
    fprintf(stderr, "CPlotLib Error: %s\n", message);
}

void cpl_warning(const char* message) {
    fprintf(stderr, "CPlotLib Warning: %s\n", message);
}

float cpl_clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float cpl_lerp(float a, float b, float t) {
    return a + t * (b - a);
}
