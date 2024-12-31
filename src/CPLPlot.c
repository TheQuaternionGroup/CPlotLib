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
    GLuint box_vbo;        // The vertex buffer object for the box
    GLuint box_vao;        // The vertex array object for the box
    size_t box_vbo_size;   // Number of vertices (4)
    float vertices[20];    // 4 vertices * (2 coords + 3 color)
    bool is_data_loaded;   // Whether the data is ready to be drawn
};

/** Forward declarations **/
static void setup_plot_box_shaders(CPLPlot* plot);
static void build_plot_box_data(CPLPlot *plot);
static void build_subplots_box_data(CPLFigure* fig, size_t rows, size_t cols, float margin);

#pragma region Helpers

static void setup_plot_box_shaders(CPLPlot* plot)
{
    if (!plot) {
        printf("Error: Could not setup plot box shaders. Plot is NULL.\n");
        return;
    }
    if (!plot->gl_data) {
        printf("Error: Could not setup plot box shaders. Plot->gl_data is NULL.\n");
        return;
    }
    if (plot->gl_data->box_vao == 0) glGenVertexArrays(1, &plot->gl_data->box_vao);
    if (plot->gl_data->box_vbo == 0) glGenBuffers(1, &plot->gl_data->box_vbo);

    glBindVertexArray(plot->gl_data->box_vao);
    glBindBuffer(GL_ARRAY_BUFFER, plot->gl_data->box_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(plot->gl_data->vertices),
                 plot->gl_data->vertices,
                 GL_STATIC_DRAW);

    // Position attribute (location=0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 
        2, 
        GL_FLOAT, 
        GL_FALSE, 
        5 * sizeof(float), 
        (void*)0);

    // Color attribute (location=1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        5 * sizeof(float), 
        (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // We have 4 corners in the box
    plot->gl_data->box_vbo_size = 4;
}

static void build_plot_box_data(CPLPlot *plot)
{
    // Allocate the gl_data struct if not done yet
    plot->gl_data = (CPL_PLOT_GL*)malloc(sizeof(CPL_PLOT_GL));
    if (!plot->gl_data) {
        printf("Error: Could not allocate memory for plot->gl_data.\n");
        return;
    }
    memset(plot->gl_data, 0, sizeof(*plot->gl_data));

    // Basic margin in NDC
    float margin = 0.1f; // 10% margin
    float left   = -1.0f + margin;
    float right  =  1.0f - margin;
    float bottom = -1.0f + margin;
    float top    =  1.0f - margin;

    // Define the box as 4 corners (CCW) with color = plot->bg_color
    float vertices[20] = {
        // x,      y,      r,                  g,                  b
        left,  top,    plot->bg_color.r, plot->bg_color.g, plot->bg_color.b,
        right, top,    plot->bg_color.r, plot->bg_color.g, plot->bg_color.b,
        right, bottom, plot->bg_color.r, plot->bg_color.g, plot->bg_color.b,
        left,  bottom, plot->bg_color.r, plot->bg_color.g, plot->bg_color.b
    };

    memcpy(plot->gl_data->vertices, vertices, sizeof(vertices));
}

/**
 * @brief Build the box data for each subplot in the figure. Each subplot is
 * laid out in normalized device coordinates, subdividing the [-1..1] range
 * in both X and Y for `rows` by `cols`.
 */
static void build_subplots_box_data(CPLFigure* fig, size_t rows, size_t cols, float margin)
{
    if (!fig) {
        printf("Error: Figure is NULL.\n");
        return;
    }

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            size_t index = row * cols + col;
            CPLPlot* plot = fig->plot[index];
            if (!plot) {
                printf("Error: Plot at index %zu is NULL.\n", index);
                continue;
            }
            // Allocate the gl_data struct
            plot->gl_data = (CPL_PLOT_GL*)malloc(sizeof(CPL_PLOT_GL));
            if (!plot->gl_data) {
                printf("Error: Could not allocate memory for subplot->gl_data.\n");
                return;
            }
            memset(plot->gl_data, 0, sizeof(*plot->gl_data));

            // Divide the [-1..1] range into col columns, row rows
            float total_margin_x = margin * (cols + 1);
            float total_margin_y = margin * (rows + 1);

            float subplot_width  = (2.0f - total_margin_x) / (float)cols;
            float subplot_height = (2.0f - total_margin_y) / (float)rows;

            // left edge of this subplot
            float left = -1.0f + margin + col * (subplot_width + margin);
            float right = left + subplot_width;
            float bottom = -1.0f + margin + row * (subplot_height + margin);
            float top = bottom + subplot_height;

            // Box color is plot->bg_color. For example, black
            float r = plot->bg_color.r;
            float g = plot->bg_color.g;
            float b = plot->bg_color.b;

            float vertices[20] = {
                // x,     y,      r, g, b
                left,  top,    r, g, b,
                right, top,    r, g, b,
                right, bottom, r, g, b,
                left,  bottom, r, g, b
            };

            memcpy(plot->gl_data->vertices, vertices, sizeof(vertices));

            // Setup the buffer/VAO
            setup_plot_box_shaders(plot);

            // Mark data loaded
            plot->gl_data->is_data_loaded = true;
        }
    }
}

#pragma endregion

#pragma region Public API

void DrawPlot(CPLPlot* plot)
{
    if (!plot || !plot->gl_data || !plot->gl_data->is_data_loaded) {
        fprintf(stderr, "Error: Could not draw plot. Plot or data is not ready.\n");
        return;
    }

    // Build an orthographic projection: -1..1 in X, -1..1 in Y
    float proj[16];
    makeOrthoMatrix(-1.0f, 1.0f, -1.0f, 1.0f, proj);

    // Use the figure's shader
    glUseProgram(plot->figure->renderer->programID);
    // Set the projection matrix
    glUniformMatrix4fv(plot->figure->renderer->proj_mat, 1, GL_FALSE, proj);

    // Bind the VAO and draw
    glBindVertexArray(plot->gl_data->box_vao);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);

    // Cleanup
    glUseProgram(0);
}

void AddPlot(CPLFigure* fig, CPLPlot* plot)
{
    if (!fig || !plot) {
        printf("Error: Could not add plot to figure. Null pointer.\n");
        return;
    }

    // Create array of one plot
    fig->plot = (CPLPlot**)malloc(sizeof(CPLPlot*));
    if (!fig->plot) {
        printf("Error: Could not allocate memory for plot array.\n");
        return;
    }
    fig->plot[0] = plot;
    fig->num_plots = 1;

    // Associate the plot with the figure
    plot->figure = fig;

    // Set some defaults
    plot->width  = fig->width;
    plot->height = fig->height;
    plot->bg_color = COLOR_BLACK; // example

    // Build the single-plot box data
    build_plot_box_data(plot);
    setup_plot_box_shaders(plot);

    // Data is loaded
    plot->gl_data->is_data_loaded = true;
}

void AddSubplots(CPLFigure* fig, size_t rows, size_t cols)
{
    if (!fig) {
        printf("Error: Could not add subplots to figure. Figure is NULL.\n");
        return;
    }

    fig->num_plots = rows * cols;

    // Allocate an array of pointers to CPLPlot
    fig->plot = (CPLPlot**)malloc(fig->num_plots * sizeof(CPLPlot*));
    if (!fig->plot) {
        printf("Error: Could not allocate memory for subplot array.\n");
        return;
    }

    // Create each subplot
    for (size_t i = 0; i < fig->num_plots; i++) {
        fig->plot[i] = (CPLPlot*)malloc(sizeof(CPLPlot));
        if (!fig->plot[i]) {
            printf("Error: Could not allocate memory for subplot %zu.\n", i);
            return;
        }
        memset(fig->plot[i], 0, sizeof(CPLPlot));

        fig->plot[i]->figure = fig;         // associate with figure
        fig->plot[i]->width  = fig->width / cols;
        fig->plot[i]->height = fig->height / rows;
        fig->plot[i]->bg_color = COLOR_BLACK; // pick a color
    }

    // Build each subplot's box data in NDC
    build_subplots_box_data(fig, rows, cols, /* margin = */ 0.1f);
}

void FreePlot(CPLPlot* plot)
{
    if (!plot) {
        fprintf(stderr, "Error: Could not free plot. Plot is NULL.\n");
        return;
    }

    // Clean up GL objects
    if (plot->gl_data) {
        if (plot->gl_data->box_vbo)
            glDeleteBuffers(1, &plot->gl_data->box_vbo);
        if (plot->gl_data->box_vao)
            glDeleteVertexArrays(1, &plot->gl_data->box_vao);
        free(plot->gl_data);
    }

    // Example if you had data arrays:
    // if (plot->x_data) free(plot->x_data);
    // if (plot->y_data) free(plot->y_data);
    // if (plot->z_data) free(plot->z_data);

    // We do *not* free(plot) here because the caller might do so.
    // But in your code, you do "free(fig->plot[i])" afterwards, so it's fine.
}

#pragma endregion
