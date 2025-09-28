#ifndef CPL_RENDERER_H
#define CPL_RENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stddef.h>
#include "CPLColors.h"

// Forward declaration
struct CPLFigure;

// Renderer structure
typedef struct CPLRenderer {
    GLFWwindow* window;
    GLuint program_id;
    GLuint proj_mat_location;
    
    // OpenGL info
    const GLubyte* renderer_name;
    const GLubyte* version;
} CPLRenderer;

// Renderer management
CPLRenderer* cpl_create_renderer(size_t width, size_t height);
void cpl_destroy_renderer(CPLRenderer* renderer);
void cpl_run_render_loop(struct CPLFigure* fig);

// OpenGL utilities
void cpl_clear_screen(Color color);
void cpl_swap_buffers(CPLRenderer* renderer);
void cpl_poll_events(void);

#endif // CPL_RENDERER_H
