#include "CPLPlot.h"
#include "CPLFigure.h"
#include "utils/cpl_gl_utils.h"
#include "utils/cpl_renderer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <GLFW/glfw3.h>
#include <GL/glew.h>

struct CPL_PLOT_GL {
    GLuint box_vbo;                 // The vertex buffer object
    GLuint box_vao;                 // The vertex array object
    size_t box_vbo_size;            // The size of the vertex buffer object
    float vertices[20];             // The vertices of the plot
    bool is_data_loaded;            // Whether or not the data has been loaded
};

static void build_plot_box_data(CPLPlot *plot)
{
    plot->gl_data = (CPL_PLOT_GL*)malloc(sizeof(CPL_PLOT_GL));
    if (!plot->gl_data)
    {
        printf("Error: Could not allocate memory for plot data.\n");
        return;
    }

    plot->gl_data->box_vbo = 0;
    plot->gl_data->box_vao = 0;

    // Use normalized device coordinates (-1 to 1)
    float margin = 0.1f; // 10% margin
    float left = -1.0f + margin;
    float right = 1.0f - margin;
    float bottom = -1.0f + margin;
    float top = 1.0f - margin;

    // Define vertices for a box counter-clockwise
    float vertices[20] = {
        // Position (x,y)    // Color (r,g,b)
        left,  top,         0.0f, 0.0f, 0.0f,  // Top left
        right, top,         0.0f, 0.0f, 0.0f,  // Top right
        right, bottom,      0.0f, 0.0f, 0.0f,  // Bottom right
        left,  bottom,      0.0f, 0.0f, 0.0f   // Bottom left
    };

    memcpy(plot->gl_data->vertices, vertices, sizeof(vertices));
}

static void setup_plot_box_shaders(CPLPlot* plot)
{
    if (!plot)
    {
        printf("Error: Could not setup plot box shaders.\n");
        return;
    }
    if (!plot->gl_data)
    {
        printf("Error: Plot data is NULL.\n");
        return;
    }
    if (plot->gl_data->box_vao == 0) glGenVertexArrays(1, &plot->gl_data->box_vao);
    if (plot->gl_data->box_vbo == 0) glGenBuffers(1, &plot->gl_data->box_vbo);

    glBindVertexArray(plot->gl_data->box_vao);
    glBindBuffer(GL_ARRAY_BUFFER, plot->gl_data->box_vbo);
    glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), plot->gl_data->vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    plot->gl_data->box_vbo_size = 4;

}

static void makeOrthoMatrix(float left, float right, float bottom, float top, float* out)
{
    float zNear=-1.f, zFar=1.f;

    if(fabsf(right-left)<1e-14f)  right=left+1.f;
    if(fabsf(top-bottom)<1e-14f)  top=bottom+1.f;

    out[0] =  2.f/(right-left);
    out[1] =  0;
    out[2] =  0;
    out[3] =  0;

    out[4] =  0;
    out[5] =  2.f/(top-bottom);
    out[6] =  0;
    out[7] =  0;

    out[8] =  0;
    out[9] =  0;
    out[10]= -2.f/(zFar-zNear);
    out[11]= 0;

    out[12]= -(right+left)/(right-left);
    out[13]= -(top+bottom)/(top-bottom);
    out[14]= -(zFar+zNear)/(zFar-zNear);
    out[15]= 1.f;
}

void DrawPlot(CPLPlot* plot)
{
    if (!plot || !plot->gl_data->is_data_loaded)
    {
        fprintf(stderr, "Error: Could not draw plot.\n");
        return;
    }

    // Get window size
    int fbW, fbH;
    glfwGetFramebufferSize(plot->figure->renderer->window, &fbW, &fbH);
    if(fbH<1) fbH=1;

    // Set up orthographic projection
    float proj[16];
    makeOrthoMatrix(-1.0f, 1.0f, -1.0f, 1.0f, proj);

    // Enable the shader program and set uniforms
    glUseProgram(plot->figure->renderer->programID);
    glUniformMatrix4fv(plot->figure->renderer->proj_mat, 1, GL_FALSE, proj);

    // Draw the box
    glBindVertexArray(plot->gl_data->box_vao);

    glDrawArrays(GL_LINE_LOOP, 0, 4);
    
    // Clean up
    glBindVertexArray(0);
    glUseProgram(0);
}

void AddPlot(CPLFigure* fig, CPLPlot* plot)
{
    if (!fig || !plot)
    {
        printf("Error: Could not add plot to figure.\n");
        return;
    }

    // create array of plots
    fig->plot = (CPLPlot**)malloc(sizeof(CPLPlot*));
    if (!fig->plot)
    {
        printf("Error: Could not allocate memory for plot.\n");
        return;
    }

    fig->plot[0] = plot;
    fig->num_plots = 1;

    // Set the figure of the plot
    plot->figure = fig;

    // Set the width and height of the plot
    plot->width = fig->width;
    plot->height = fig->height;

    // Set the background color of the plot
    plot->bg_color = COLOR_BLACK;

    // set up the plot box data
    build_plot_box_data(plot);

    // set up the plot box shaders
    setup_plot_box_shaders(plot);

    // set the data loaded flag
    plot->gl_data->is_data_loaded = true;
    
}

void AddSubplots(CPLFigure* fig, size_t rows, size_t cols)
{
    if (!fig)
    {
        printf("Error: Could not add subplots to figure.\n");
        return;
    }

    // Allocate memory for the plots
    fig->plot = (CPLPlot**)realloc(fig->plot, rows * cols * sizeof(CPLPlot*));
    if (!fig->plot)
    {
        printf("Error: Could not reallocate memory for subplots.\n");
        return;
    }
    fig->num_plots = rows * cols;
}

void SetXRange(CPLPlot* plot, double x_range[2])
{
    if (!plot)
    {
        fprintf(stderr, "Error: Could not set x-axis range.\n");
        return;
    }
    if (!x_range)
    {
        fprintf(stderr, "Error: x_range is NULL.\n");
        return;
    }
    memcpy(plot->x_range, x_range, 2 * sizeof(double));
}

void SetYRange(CPLPlot* plot, double y_range[2])
{
    if (!plot)
    {
        fprintf(stderr, "Error: Could not set y-axis range.\n");
        return;
    }
    if (!y_range)
    {
        fprintf(stderr, "Error: y_range is NULL.\n");
        return;
    }
    memcpy(plot->y_range, y_range, 2 * sizeof(double));
}

void SetZRange(CPLPlot* plot, double z_range[2])
{
    if (!plot)
    {
        fprintf(stderr, "Error: Could not set z-axis range.\n");
        return;
    }
    if (!z_range)
    {
        fprintf(stderr, "Error: z_range is NULL.\n");
        return;
    }
    memcpy(plot->z_range, z_range, 2 * sizeof(double));
}

void SetXData(CPLPlot* plot, double* x_data, size_t num_xticks)
{
    if (!plot)
    {
        fprintf(stderr, "Error: Could not set x-axis data.\n");
        return;
    }
    if (!x_data)
    {
        fprintf(stderr, "Error: x_data is NULL.\n");
        return;
    }
    // allocate memory for x_data if it hasn't been allocated yet
    if (!plot->x_data)
    {
        plot->x_data = (double*)malloc(num_xticks * sizeof(double));
        if (!plot->x_data)
        {
            fprintf(stderr, "Error: Could not allocate memory for x_data.\n");
            return;
        }
    }
    memcpy(plot->x_data, x_data, num_xticks * sizeof(double));
    plot->num_xticks = num_xticks;
}

void SetYData(CPLPlot* plot, double* y_data, size_t num_yticks)
{
    if (!plot)
    {
        fprintf(stderr, "Error: Could not set y-axis data.\n");
        return;
    }
    if (!y_data)
    {
        fprintf(stderr, "Error: y_data is NULL.\n");
        return;
    }
    // allocate memory for y_data if it hasn't been allocated yet
    if (!plot->y_data)
    {
        plot->y_data = (double*)malloc(num_yticks * sizeof(double));
        if (!plot->y_data)
        {
            fprintf(stderr, "Error: Could not allocate memory for y_data.\n");
            return;
        }
    }
    memcpy(plot->y_data, y_data, num_yticks * sizeof(double));
    plot->num_yticks = num_yticks;
}

void SetZData(CPLPlot* plot, double* z_data, size_t num_zticks)
{
    if (!plot)
    {
        fprintf(stderr, "Error: Could not set z-axis data.\n");
        return;
    }
    if (!z_data)
    {
        fprintf(stderr, "Error: z_data is NULL.\n");
        return;
    }
    // allocate memory for z_data if it hasn't been allocated yet
    if (!plot->z_data)
    {
        plot->z_data = (double*)malloc(num_zticks * sizeof(double));
        if (!plot->z_data)
        {
            fprintf(stderr, "Error: Could not allocate memory for z_data.\n");
            return;
        }
    }
    memcpy(plot->z_data, z_data, num_zticks * sizeof(double));
    plot->num_zticks = num_zticks;
}


void FreePlot(CPLPlot* plot)
{
    if (!plot)
    {
        fprintf(stderr, "Error: Could not free plot.\n");
        return;
    }

    // free any memory associated with the plot
    if (plot->x_data) free(plot->x_data);
    if (plot->y_data) free(plot->y_data);
    if (plot->z_data) free(plot->z_data);

    // Free the plot
    free(plot);
    
}