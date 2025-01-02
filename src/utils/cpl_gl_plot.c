#include "C_headers/utils/cpl_gl_plot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  float margin;            // Margin around the plot
};

// Helper function to ensure plot->gl_data is allocated
static void ensure_gl_data(CPLPlot *plot) {
  if (!plot->gl_data) {
    plot->gl_data = (CPL_PLOT_GL *)calloc(1, sizeof(CPL_PLOT_GL));
    if (!plot->gl_data) {
      fprintf(stderr, "Error: Could not allocate memory for plot->gl_data.\n");
    }
  }
}

void initialize_gl_data(CPLPlot *plot) {
  ensure_gl_data(plot);
}

void generate_box_vertices(CPLPlot *plot, float left, float right, float bottom, float top) {
  float r = plot->bg_color.r;
  float g = plot->bg_color.g;
  float b = plot->bg_color.b;

  float vertices[20] = {
      // x,    y,      r, g, b
      left,  top,    r, g, b,
      right, top,    r, g, b,
      right, bottom, r, g, b,
      left,  bottom, r, g, b
  };

  memcpy(plot->gl_data->vertices, vertices, sizeof(vertices));
}

float normalize_coordinate(double value, double min, double max, float plot_start, float plot_size) {
  if (max - min == 0) {
    fprintf(stderr, "Error: Division by zero in normalization.\n");
    return plot_start;
  }
  float normalized = ((value - min) / (float)(max - min)) * plot_size + plot_start;
  if (normalized < plot_start) normalized = plot_start;
  if (normalized > plot_start + plot_size) normalized = plot_start + plot_size;
  return normalized;
}

void setup_plot_box_shaders(CPLPlot *plot) {
  ensure_gl_data(plot);
  if (!plot->gl_data) return;

  if (plot->gl_data->box_vao == 0)
    glGenVertexArrays(1, &plot->gl_data->box_vao);
  if (plot->gl_data->box_vbo == 0)
    glGenBuffers(1, &plot->gl_data->box_vbo);

  glBindVertexArray(plot->gl_data->box_vao);
  glBindBuffer(GL_ARRAY_BUFFER, plot->gl_data->box_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(plot->gl_data->vertices),
               plot->gl_data->vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                        5 * sizeof(float), (void *)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                        5 * sizeof(float), (void *)(2 * sizeof(float)));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  plot->gl_data->box_vbo_size = 4;
}

void build_plot_box_data(CPLPlot *plot) {
  ensure_gl_data(plot);
  if (!plot->gl_data) return;

  if (plot->gl_data->margin == 0.0f) {
    plot->gl_data->margin = 0.1f;
  }

  float margin = plot->gl_data->margin;
  float left = -1.0f + margin;
  float right = 1.0f - margin;
  float bottom = -1.0f + margin;
  float top = 1.0f - margin;

  generate_box_vertices(plot, left, right, bottom, top);
}

void build_subplots_box_data(CPLFigure *fig, size_t rows, size_t cols, float margin) {
  if (!fig) {
    fprintf(stderr, "Error: Figure is NULL.\n");
    return;
  }

  for (size_t row = 0; row < rows; ++row) {
    for (size_t col = 0; col < cols; ++col) {
      size_t index = row * cols + col;
      if (index >= fig->num_plots) {
        fprintf(stderr, "Error: Plot index %zu out of range.\n", index);
        continue;
      }
      CPLPlot *plot = fig->plot[index];
      if (!plot) {
        fprintf(stderr, "Error: Plot at index %zu is NULL.\n", index);
        continue;
      }

      ensure_gl_data(plot);
      if (!plot->gl_data) return;

      plot->gl_data->margin = margin;

      float total_margin_x = margin * (cols + 1);
      float total_margin_y = margin * (rows + 1);
      float subplot_width = (2.0f - total_margin_x) / (float)cols;
      float subplot_height = (2.0f - total_margin_y) / (float)rows;

      float left = -1.0f + margin + col * (subplot_width + margin);
      float right = left + subplot_width;
      float bottom_plot = -1.0f + margin + row * (subplot_height + margin);
      float top_plot = bottom_plot + subplot_height;

      generate_box_vertices(plot, left, right, bottom_plot, top_plot);
      setup_plot_box_shaders(plot);
      plot->gl_data->is_box_data_loaded = true;
    }
  }
}

void setup_plot_line_shaders(CPLPlot *plot, CPLLine *line) {
  if (!plot->gl_data || !line) {
    fprintf(stderr,
            "Error: plot->gl_data or line is NULL in setup_plot_line_shaders.\n");
    return;
  }

  if (line->vao == 0) glGenVertexArrays(1, &line->vao);
  if (line->vbo == 0) glGenBuffers(1, &line->vbo);

  glBindVertexArray(line->vao);
  glBindBuffer(GL_ARRAY_BUFFER, line->vbo);
  glBufferData(GL_ARRAY_BUFFER, line->vbo_size * 5 * sizeof(float),
               line->vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                        5 * sizeof(float), (void *)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                        5 * sizeof(float), (void *)(2 * sizeof(float)));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

// Helper to allocate line and handle common checks
static CPLLine *create_new_line(CPLPlot *plot, size_t num_points) {
  CPLLine *new_lines =
      (CPLLine *)realloc(plot->lines, (plot->num_lines + 1) * sizeof(CPLLine));
  if (!new_lines) {
    fprintf(stderr, "Error: Could not allocate memory for new line.\n");
    return NULL;
  }
  plot->lines = new_lines;
  CPLLine *current_line = &plot->lines[plot->num_lines];
  plot->num_lines++;

  current_line->vbo = 0;
  current_line->vao = 0;
  current_line->vbo_size = num_points;
  current_line->is_data_loaded = false;
  current_line->vertices = (float *)malloc(num_points * 5 * sizeof(float));
  if (!current_line->vertices) {
    fprintf(stderr, "Error: Could not allocate memory for line vertices.\n");
    plot->num_lines--;
    return NULL;
  }
  return current_line;
}

// Helper to fill line data
static bool fill_line_data(CPLPlot *plot, CPLLine *line,
                           double *x_arr, double *y_arr, double *t_arr,
                           size_t num_points, ColorCallback color_fn,
                           void *user_data, Color line_color, bool use_t) {
  double x_min = plot->x_range[0];
  double x_max = plot->x_range[1];
  double y_min = plot->y_range[0];
  double y_max = plot->y_range[1];

  if (!line || x_max - x_min == 0 || y_max - y_min == 0) {
    fprintf(stderr, "Error: Invalid range or null line.\n");
    return false;
  }

  float plot_left   = plot->gl_data->vertices[0];
  float plot_top    = plot->gl_data->vertices[1];
  float plot_right  = plot->gl_data->vertices[5];
  float plot_bottom = plot->gl_data->vertices[11];
  float plot_width  = plot_right - plot_left;
  float plot_height = plot_top - plot_bottom;

  for (size_t i = 0; i < num_points; i++) {
    float nx = normalize_coordinate(x_arr[i], x_min, x_max, plot_left, plot_width);
    float ny = normalize_coordinate(y_arr[i], y_min, y_max, plot_bottom, plot_height);

    Color c = line_color;
    if (color_fn) {
      double param = use_t ? t_arr[i] : x_arr[i];
      c = color_fn(param, user_data);
    }
    line->vertices[i * 5 + 0] = nx;
    line->vertices[i * 5 + 1] = ny;
    line->vertices[i * 5 + 2] = c.r;
    line->vertices[i * 5 + 3] = c.g;
    line->vertices[i * 5 + 4] = c.b;
  }
  return true;
}

void build_plot_line_data(CPLPlot *plot, double *x_arr, double *y_arr,
                          size_t num_points, Color line_color,
                          ColorCallback color_fn, void *user_data) {
  if (!plot->gl_data) {
    fprintf(stderr, "Error: plot->gl_data is NULL in build_plot_line_data.\n");
    return;
  }

  CPLLine *current_line = create_new_line(plot, num_points);
  if (!current_line) return;

  if (!fill_line_data(plot, current_line, x_arr, y_arr,
                      NULL, num_points, color_fn, user_data,
                      line_color, false)) {
    free(current_line->vertices);
    plot->num_lines--;
    return;
  }

  setup_plot_line_shaders(plot, current_line);
  current_line->is_data_loaded = true;
}

void build_plot_curve_data(CPLPlot *plot, double *t_arr, double *x_arr,
                           double *y_arr, size_t num_points, Color line_color,
                           ColorCallback color_fn, void *user_data) {
  if (!plot->gl_data) {
    fprintf(stderr, "Error: plot->gl_data is NULL in build_plot_line_data.\n");
    return;
  }

  CPLLine *current_line = create_new_line(plot, num_points);
  if (!current_line) return;

  if (!fill_line_data(plot, current_line, x_arr, y_arr,
                      t_arr, num_points, color_fn, user_data,
                      line_color, true)) {
    free(current_line->vertices);
    plot->num_lines--;
    return;
  }

  setup_plot_line_shaders(plot, current_line);
  current_line->is_data_loaded = true;
}