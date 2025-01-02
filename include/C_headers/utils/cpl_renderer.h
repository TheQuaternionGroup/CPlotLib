#ifndef CPL_RENDERER_H
#define CPL_RENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../CPLCore.h"

struct CPLRenderer
{
    GLFWwindow* window;         // The window in which the plot is displayed

    const GLubyte* renderer;    // The renderer used to display the plot
    const GLubyte* version;     // The version of OpenGL used to display the plot

    GLuint programID;           // The shader program used to render the plot
    GLuint vbo;                 // The vertex buffer object used to store the plot data
    GLuint vao;                 // The vertex array object used to store the plot data
    GLuint vs;                  // The vertex shader used to render the plot
    GLuint fs;                  // The fragment shader used to render the plot
    GLuint proj_mat;    // The location of the projection matrix in the shader program
};

/*!
* @brief Creates and sets up the OpenGL and GLFW context for the plot.
* @return A pointer to the renderer.
*/
CPLRenderer* cpl_init_renderer(size_t width, size_t height);

/*!
* @brief Frees the memory associated with the renderer.
* @param renderer The renderer to free.
*/
void cpl_free_renderer(CPLRenderer* renderer);

#endif // CPL_RENDERER_H