#include "CPLRenderer.h"
#include "CPLShader.h"
#include "CPLUtils.h"
#include "CPLColors.h"
#include "CPLPlot.h"

#include <stdio.h>
#include <stdlib.h>

CPLRenderer* cpl_create_renderer(size_t width, size_t height) {
    CPLRenderer* renderer = malloc(sizeof(CPLRenderer));
    if (!renderer) {
        fprintf(stderr, "Failed to allocate renderer\n");
        return NULL;
    }
    
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        free(renderer);
        return NULL;
    }
    
    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    // Create window
    renderer->window = glfwCreateWindow(width, height, "CPlotLib", NULL, NULL);
    if (!renderer->window) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        free(renderer);
        return NULL;
    }
    
    // Set window properties
    glfwSetWindowSizeLimits(renderer->window, width, height, width, height);
    glfwMakeContextCurrent(renderer->window);
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwDestroyWindow(renderer->window);
        glfwTerminate();
        free(renderer);
        return NULL;
    }
    
    // Get OpenGL info
    renderer->renderer_name = glGetString(GL_RENDERER);
    renderer->version = glGetString(GL_VERSION);
    
    // Create shader program
    renderer->program_id = cpl_create_shader_program();
    if (renderer->program_id == 0) {
        fprintf(stderr, "Failed to create shader program\n");
        glfwDestroyWindow(renderer->window);
        glfwTerminate();
        free(renderer);
        return NULL;
    }
    
    // Get uniform locations
    renderer->proj_mat_location = glGetUniformLocation(renderer->program_id, "proj_mat");
    
    // Enable OpenGL features
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Performance optimizations
    glEnable(GL_MULTISAMPLE);  // Enable anti-aliasing
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    
    // Disable unnecessary features for 2D plotting
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    
    // Set initial viewport
    glViewport(0, 0, width, height);
    
    return renderer;
}

void cpl_destroy_renderer(CPLRenderer* renderer) {
    if (!renderer) return;
    
    if (renderer->program_id) {
        glDeleteProgram(renderer->program_id);
    }
    
    if (renderer->window) {
        glfwDestroyWindow(renderer->window);
    }
    
    glfwTerminate();
    free(renderer);
}

void cpl_run_render_loop(struct CPLFigure* fig) {
    if (!fig || !fig->renderer) return;
    
    // Pre-calculate projection matrix (it's the same for all plots)
    float proj[16];
    cpl_make_ortho_matrix(-1.0f, 1.0f, -1.0f, 1.0f, proj);
    
    // Cache shader program and uniform location
    GLuint program_id = fig->renderer->program_id;
    GLint proj_mat_location = fig->renderer->proj_mat_location;
    
    while (!glfwWindowShouldClose(fig->renderer->window)) {
        // Clear screen
        cpl_clear_screen(fig->bg_color);
        
        // Get framebuffer size
        int fb_width, fb_height;
        glfwGetFramebufferSize(fig->renderer->window, &fb_width, &fb_height);
        
        // Set up OpenGL state once per frame
        glUseProgram(program_id);
        glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, proj);
        
        // Set resolution uniform for shader-based line thickness
        GLint resolution_location = glGetUniformLocation(program_id, "resolution");
        if (resolution_location != -1) {
            glUniform2f(resolution_location, (float)fb_width, (float)fb_height);
        }
        
        // Render all plots
        for (size_t i = 0; i < fig->num_plots; i++) {
            if (fig->plots[i]) {
                // Set viewport based on subplot layout
                if (fig->plots[i]->is_subplot && fig->plots[i]->subplot_layout) {
                    // Calculate viewport for this subplot
                    int viewport_x = (int)(fig->plots[i]->subplot_layout->left * fb_width);
                    int viewport_y = (int)(fig->plots[i]->subplot_layout->bottom * fb_height);
                    int viewport_w = (int)((fig->plots[i]->subplot_layout->right - fig->plots[i]->subplot_layout->left) * fb_width);
                    int viewport_h = (int)((fig->plots[i]->subplot_layout->top - fig->plots[i]->subplot_layout->bottom) * fb_height);
                    
                    glViewport(viewport_x, viewport_y, viewport_w, viewport_h);
                } else {
                    // Full screen for regular plots
                    glViewport(0, 0, fb_width, fb_height);
                }
                
                cpl_render_plot(fig->plots[i]);
            }
        }
        
        // Check for ESC key
        if (glfwGetKey(fig->renderer->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(fig->renderer->window, GLFW_TRUE);
        }
        
        cpl_poll_events();
        cpl_swap_buffers(fig->renderer);
    }
}

void cpl_clear_screen(Color color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void cpl_swap_buffers(CPLRenderer* renderer) {
    if (renderer && renderer->window) {
        glfwSwapBuffers(renderer->window);
    }
}

void cpl_poll_events(void) {
    glfwPollEvents();
}
