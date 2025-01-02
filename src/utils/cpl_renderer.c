#include "C_headers/utils/cpl_renderer.h"
#include "C_headers/utils/cpl_gl_utils.h"
#include "C_headers/CPLColors.h"

#include <stdlib.h>
#include <stdio.h>

CPLRenderer* cpl_init_renderer(size_t width, size_t height)
{
    // Allocate memory for the plot
    CPLRenderer* renderer = (CPLRenderer*)malloc(sizeof(CPLRenderer));
    if (!renderer)
    {
        printf("Error: Could not allocate memory for renderer.\n");
        return NULL;
    }
    // Initialize GLFW
    if (!glfwInit())
    {
        printf("Error: Could not initialize GLFW.\n");
        free(renderer);
        return NULL;
    }
    // initialze the renderer attributes
    renderer->renderer = NULL;
    renderer->version = NULL;

    // Set the OpenGL version to 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint( GLFW_SAMPLES, 4 );

    // Create a window 
    // Default size is 800x600
    renderer->window = glfwCreateWindow(width, height, "", NULL, NULL);

    if (!renderer->window)
    {
        printf("Error: Could not create window.\n");
        free(renderer);
        glfwTerminate();
        return NULL;
    }
    // We must specify 3.2 core if on Apple OS X -- other O/S can specify 
    // set anti-aliasing factor to make diagonal edges appear less jagged
    // For now, no window resizing
    glfwSetWindowSizeLimits(renderer->window, width, height, width, height);

    // Make the window's context current
    glfwMakeContextCurrent(renderer->window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        printf("Error: Could not initialize GLEW.\n");
        free(renderer);
        glfwTerminate();
        return NULL;
    }

    // Get the renderer and version
    renderer->renderer = glGetString(GL_RENDERER);
    renderer->version = glGetString(GL_VERSION);


    // Set up the shaders
    SetUpShaders(&renderer->programID, &renderer->vbo, &renderer->vao, &renderer->vs, &renderer->fs);

    renderer->proj_mat = glGetUniformLocation(renderer->programID, "proj_mat");
    if(renderer->proj_mat < 0)
    {
        fprintf(stderr, "Warning: uniform 'u_projection' not found.\n");
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    return renderer;
}

void cpl_free_renderer(CPLRenderer* renderer)
{
    if (!renderer)
    {
        printf("Error: Could not free renderer.\n");
        return;
    }

    glDeleteProgram(renderer->programID);
    glDeleteBuffers(1, &renderer->vbo);
    glDeleteVertexArrays(1, &renderer->vao);
    glDeleteShader(renderer->vs);
    glDeleteShader(renderer->fs);
    // Free the plot
    free(renderer);
}