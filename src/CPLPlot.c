#include "C_headers/CPLPlot.h"
#include "C_headers/CPLFigure.h"
#include "C_headers/utils/cpl_gl_renderer.h"
#include "C_headers/utils/cpl_gl_utils.h"
#include "C_headers/utils/cpl_gl_plot.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Structure representing a single line within a plot.
struct CPLLine {
    GLuint vbo;          // Vertex Buffer Object for the line
    GLuint vao;          // Vertex Array Object for the line
    size_t vbo_size;     // Number of vertices
    float *vertices;     // Vertex data (positions and colors)
    bool is_data_loaded; // Flag indicating if data is loaded
};

// Structure representing OpenGL data for a plot.
struct CPL_PLOT_GL {
    GLuint box_vbo;          // Vertex Buffer Object for the box
    GLuint box_vao;          // Vertex Array Object for the box
    size_t box_vbo_size;     // Number of vertices (4)
    float vertices[20];      // 4 vertices * (2 coords + 3 color)
    bool is_box_data_loaded; // Whether the box data is ready to be drawn

    float margin; // Margin around the plot
};

struct CPL_GRID_GL {
    GLuint grid_vbo;          // Vertex Buffer Object for the grid
    GLuint grid_vao;          // Vertex Array Object for the grid
    size_t grid_vbo_size;     // Number of vertices
    float *vertices;          // Vertex data (positions and colors)
    bool is_grid_data_loaded; // Whether the grid data is ready to be drawn
};

#pragma region Private API

static void ReportError(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
}

static void CleanupCPLLine(CPLLine *line) {
    if (line->vbo)
        glDeleteBuffers(1, &line->vbo);
    if (line->vao)
        glDeleteVertexArrays(1, &line->vao);
    if (line->vertices)
        free(line->vertices);
}

static void CleanupCPL_PLOT_GL(CPL_PLOT_GL *gl_data) {
    if (gl_data->box_vbo)
        glDeleteBuffers(1, &gl_data->box_vbo);
    if (gl_data->box_vao)
        glDeleteVertexArrays(1, &gl_data->box_vao);
}

static void CleanupCPL_GRID_GL(CPL_GRID_GL *grid_data) {
    if (grid_data->grid_vbo)
        glDeleteBuffers(1, &grid_data->grid_vbo);
    if (grid_data->grid_vao)
        glDeleteVertexArrays(1, &grid_data->grid_vao);
    if (grid_data->vertices)
        free(grid_data->vertices);
}

void DrawPlot(CPLPlot *plot) {
    if (!plot || !plot->gl_data || !plot->gl_data->is_box_data_loaded) {
        ReportError("Could not draw plot. Plot or data is not ready.");
        return;
    }

    // Build an orthographic projection: -1..1 in X, -1..1 in Y.
    float proj[16];
    makeOrthoMatrix(-1.0f, 1.0f, -1.0f, 1.0f, proj);

    // Use the figure's shader program.
    glUseProgram(plot->figure->renderer->programID);
    // Set the projection matrix uniform.
    glUniformMatrix4fv(plot->figure->renderer->proj_mat, 1, GL_FALSE, proj);

    // Draw the bounding box.
    glBindVertexArray(plot->gl_data->box_vao);
    glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)plot->gl_data->box_vbo_size);
    glBindVertexArray(0);

    // Draw the grid if enabled.
    if (plot->show_grid && plot->grid_data && plot->grid_data->is_grid_data_loaded) {
        glBindVertexArray(plot->grid_data->grid_vao);
        glDrawArrays(GL_LINES, 0, (GLsizei)plot->grid_data->grid_vbo_size);
        glBindVertexArray(0);
    }

    // Draw each loaded line.
    for (size_t i = 0; i < plot->num_lines; i++) {
        CPLLine *line = &plot->lines[i];
        if (line->is_data_loaded && line->vbo_size > 0) {
            glBindVertexArray(line->vao);
            glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)line->vbo_size);
            glBindVertexArray(0);
        }
    }

    // Cleanup: Unbind the shader program.
    glUseProgram(0);
}

void FreePlot(CPLPlot *plot) {
    if (!plot) {
        ReportError("Could not free plot. Plot is NULL.");
        return;
    }

    // Clean up OpenGL objects and line data.
    if (plot->gl_data) {
        CleanupCPL_PLOT_GL(plot->gl_data);

        // Free each line's resources.
        for (size_t i = 0; i < plot->num_lines; i++) {
            CleanupCPLLine(&plot->lines[i]);
        }

        // Free the lines array.
        free(plot->lines);
        plot->lines = NULL;

        // Free the OpenGL data structure.
        free(plot->gl_data);
        plot->gl_data = NULL;
    }

    // Free grid data if it exists
    if (plot->grid_data) {
        CleanupCPL_GRID_GL(plot->grid_data);
        free(plot->grid_data);
        plot->grid_data = NULL;
    }

    // **Do not free(plot) here to avoid double free.**
}

#pragma endregion

#pragma region Public API

CPLPlot *AddPlot(CPLFigure *fig) {
    if (!fig) {
        ReportError("Could not add plot to figure. Null pointer.");
        return NULL;
    }

    // Allocate memory for the new plot.
    CPLPlot *plot = (CPLPlot *)calloc(1, sizeof(CPLPlot));
    if (!plot) {
        ReportError("Could not allocate memory for plot.");
        return NULL;
    }

    // Reallocate the plot array to accommodate the new plot.
    CPLPlot **new_plots = (CPLPlot **)realloc(fig->plot, (fig->num_plots + 1) * sizeof(CPLPlot *));
    if (!new_plots) {
        ReportError("Could not allocate memory for plot array.");
        free(plot);
        return NULL;
    }
    fig->plot = new_plots;
    fig->plot[fig->num_plots] = plot;
    fig->num_plots += 1;

    // Associate the plot with the figure.
    plot->figure = fig;

    // Set default dimensions and background color.
    plot->width = fig->width;
    plot->height = fig->height;
    plot->bg_color = COLOR_BLACK; // Default background color.

    plot->show_grid = true;

    // Build and setup bounding box data.
    build_plot_box_data(plot);
    setup_plot_box_shaders(plot);

    // Initialize lines array.
    plot->lines = NULL;
    plot->num_lines = 0;

    // Mark data as loaded.
    plot->gl_data->is_box_data_loaded = true;

    return plot;
}

void AddSubplots(CPLFigure *fig, size_t rows, size_t cols) {
    if (!fig) {
        ReportError("Could not add subplots to figure. Figure is NULL.");
        return;
    }

    size_t total_plots = rows * cols;
    if (total_plots == 0) {
        ReportError("Number of rows and columns must be greater than zero.");
        return;
    }

    // Reallocate the plot array to accommodate new subplots.
    CPLPlot **new_plots = (CPLPlot **)realloc(
        fig->plot, (fig->num_plots + total_plots) * sizeof(CPLPlot *));
    if (!new_plots) {
        ReportError("Could not allocate memory for subplot array.");
        return;
    }
    fig->plot = new_plots;

    // Allocate and initialize each subplot.
    for (size_t i = 0; i < total_plots; i++) {
        CPLPlot *new_plot = (CPLPlot *)calloc(1, sizeof(CPLPlot));
        if (!new_plot) {
            fprintf(stderr, "Error: Could not allocate memory for subplot %zu.\n", i);
            // Free previously allocated subplots in this function to prevent leaks.
            for (size_t j = 0; j < i; j++) {
                FreePlot(fig->plot[fig->num_plots + j]);
                free(fig->plot[fig->num_plots + j]);
                fig->plot[fig->num_plots + j] = NULL;
            }
            fig->num_plots += (i > 0) ? i : 0;
            return;
        }

        // Associate the subplot with the figure.
        new_plot->figure = fig;

        // Set default dimensions and background color based on grid layout.
        new_plot->width = fig->width / (float)cols;
        new_plot->height = fig->height / (float)rows;
        new_plot->bg_color = COLOR_BLACK; // Default background color.

        // Initialize lines array.
        new_plot->lines = NULL;
        new_plot->num_lines = 0;

        new_plot->show_grid = true;

        fig->plot[fig->num_plots + i] = new_plot;
    }

    fig->num_plots += total_plots;

    // Build each subplot's bounding box data in Normalized Device Coordinates (NDC).
    build_subplots_box_data(fig, rows, cols, 0.1f); // 10% margin between subplots.
}

CPLAPI void Plot(CPLPlot *plot, double *x_arr, double *y_arr, size_t num_points,
                Color line_color, ColorCallback color_fn, void *user_data) {
    if (!plot || !x_arr || !y_arr || num_points == 0) {
        ReportError("Could not plot. Invalid input.");
        return;
    }

    // Set default ranges if not set.
    if (plot->x_range[0] == plot->x_range[1]) {
        plot->x_range[0] = 0.0;
        plot->x_range[1] = 1.0;
    }
    if (plot->y_range[0] == plot->y_range[1]) {
        plot->y_range[0] = 0.0;
        plot->y_range[1] = 1.0;
    }

    // Setup the grid.
    double x_range = x_arr[num_points - 1] - x_arr[0];
    size_t xi = round(x_range + 0.5) + 1;

    build_grid_data(plot, xi, xi);
    setup_grid_shaders(plot);

    // Build the line data.
    build_plot_line_data(plot, x_arr, y_arr, num_points, line_color, color_fn, user_data);
}

void PlotParamCurve(CPLPlot *plot, double *t_arr, double *x_arr, double *y_arr,
                   size_t num_points, Color line_color, ColorCallback color_fn,
                   void *user_data) {
    if (!plot || !t_arr || !x_arr || !y_arr || num_points == 0) {
        ReportError("Could not plot parametric curve. Invalid input.");
        return;
    }

    // Set default ranges if not set.
    if (plot->x_range[0] == plot->x_range[1]) {
        plot->x_range[0] = 0.0;
        plot->x_range[1] = 1.0;
    }
    if (plot->y_range[0] == plot->y_range[1]) {
        plot->y_range[0] = 0.0;
        plot->y_range[1] = 1.0;
    }

    double x_range = x_arr[num_points - 1] - x_arr[0];
    size_t xi = round(x_range + 0.5) + 1;

    build_grid_data(plot, xi, xi);
    setup_grid_shaders(plot);

    // Build the line data.
    build_plot_curve_data(plot, t_arr, x_arr, y_arr, num_points, line_color,
                         color_fn, user_data);
}

void SetXRange(CPLPlot *plot, double x_range[2]) {
    if (!plot || !x_range) {
        ReportError("Could not set x-range. Invalid input.");
        return;
    }

    memcpy(plot->x_range, x_range, 2 * sizeof(double));
}

void SetYRange(CPLPlot *plot, double y_range[2]) {
    if (!plot || !y_range) {
        ReportError("Could not set y-range. Invalid input.");
        return;
    }

    memcpy(plot->y_range, y_range, 2 * sizeof(double));
}

void SetZRange(CPLPlot *plot, double z_range[2]) {
    if (!plot || !z_range) {
        ReportError("Could not set z-range. Invalid input.");
        return;
    }

    memcpy(plot->z_range, z_range, 2 * sizeof(double));
}

void ShowGrid(CPLPlot* plot, bool show) {
    if (!plot) {
        ReportError("Could not show grid. Plot is NULL.");
        return;
    }

    plot->show_grid = show;
}

#pragma endregion