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

#pragma region Private API

void DrawPlot(CPLPlot *plot) {
  if (!plot || !plot->gl_data || !plot->gl_data->is_box_data_loaded) {
    fprintf(stderr, "Error: Could not draw plot. Plot or data is not ready.\n");
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

  // Draw each line associated with this plot.
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
    fprintf(stderr, "Error: Could not free plot. Plot is NULL.\n");
    return;
  }

  // Clean up OpenGL objects and line data.
  if (plot->gl_data) {
    if (plot->gl_data->box_vbo)
      glDeleteBuffers(1, &plot->gl_data->box_vbo);
    if (plot->gl_data->box_vao)
      glDeleteVertexArrays(1, &plot->gl_data->box_vao);

    // Free each line's resources.
    for (size_t i = 0; i < plot->num_lines; i++) {
      CPLLine *line = &plot->lines[i];
      if (line->vbo)
        glDeleteBuffers(1, &line->vbo);
      if (line->vao)
        glDeleteVertexArrays(1, &line->vao);
      if (line->vertices)
        free(line->vertices);
    }

    // Free the lines array.
    free(plot->lines);
    plot->lines = NULL;

    // Free the OpenGL data structure.
    free(plot->gl_data);
    plot->gl_data = NULL;
  }

  // **Do not free(plot) here to avoid double free.**
}

#pragma endregion

#pragma region Public API

CPLPlot *AddPlot(CPLFigure *fig) {
  if (!fig) {
    fprintf(stderr, "Error: Could not add plot to figure. Null pointer.\n");
    return NULL;
  }

  // Allocate memory for the new plot.
  CPLPlot *plot = (CPLPlot *)calloc(1, sizeof(CPLPlot));
  if (!plot) {
    fprintf(stderr, "Error: Could not allocate memory for plot.\n");
    return NULL;
  }

  // Reallocate the plot array to accommodate the new plot.
  CPLPlot **new_plots =
      (CPLPlot **)realloc(fig->plot, (fig->num_plots + 1) * sizeof(CPLPlot *));
  if (!new_plots) {
    fprintf(stderr, "Error: Could not allocate memory for plot array.\n");
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

  // Build the single-plot bounding box data.
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
    fprintf(stderr,
            "Error: Could not add subplots to figure. Figure is NULL.\n");
    return;
  }

  size_t total_plots = rows * cols;
  if (total_plots == 0) {
    fprintf(stderr,
            "Error: Number of rows and columns must be greater than zero.\n");
    return;
  }

  // Reallocate the plot array to accommodate new subplots.
  CPLPlot **new_plots = (CPLPlot **)realloc(
      fig->plot, (fig->num_plots + total_plots) * sizeof(CPLPlot *));
  if (!new_plots) {
    fprintf(stderr, "Error: Could not allocate memory for subplot array.\n");
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

    fig->plot[fig->num_plots + i] = new_plot;
  }

  fig->num_plots += total_plots;

  // Build each subplot's bounding box data in Normalized Device Coordinates
  // (NDC).
  build_subplots_box_data(fig, rows, cols,
                          0.1f); // 10% margin between subplots.
}

CPLAPI void Plot(CPLPlot *plot, double *x_arr, double *y_arr, size_t num_points,
                 Color line_color, ColorCallback color_fn, void *user_data) {
  if (!plot || !x_arr || !y_arr || num_points == 0) {
    fprintf(stderr, "Error: Could not plot. Invalid input.\n");
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

  // Build the line data.
  build_plot_line_data(plot, x_arr, y_arr, num_points, line_color, color_fn,
                       user_data);
}

void PlotParamCurve(CPLPlot *plot, double *t_arr, double *x_arr, double *y_arr,
                    size_t num_points, Color line_color, ColorCallback color_fn,
                    void *user_data) {
  if (!plot || !t_arr || !x_arr || !y_arr || num_points == 0) {
    fprintf(stderr, "Error: Could not plot parametric curve. Invalid input.\n");
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

  // Build the line data.
  build_plot_curve_data(plot, t_arr, x_arr, y_arr, num_points, line_color,
                        color_fn, user_data);
}

void SetXRange(CPLPlot *plot, double x_range[2]) {
  if (!plot || !x_range) {
    fprintf(stderr, "Error: Could not set x-range. Invalid input.\n");
    return;
  }

  memcpy(plot->x_range, x_range, 2 * sizeof(double));
}

void SetYRange(CPLPlot *plot, double y_range[2]) {
  if (!plot || !y_range) {
    fprintf(stderr, "Error: Could not set y-range. Invalid input.\n");
    return;
  }

  memcpy(plot->y_range, y_range, 2 * sizeof(double));
}

void SetZRange(CPLPlot *plot, double z_range[2]) {
  if (!plot || !z_range) {
    fprintf(stderr, "Error: Could not set z-range. Invalid input.\n");
    return;
  }

  memcpy(plot->z_range, z_range, 2 * sizeof(double));
}

#pragma endregion
