#ifndef CPL_GL_PLOT_H
#define CPL_GL_PLOT_H

#include "../CPLPlot.h"
#include "../CPLFigure.h"
#include "cpl_gl_utils.h"

#include <GL/glew.h>

// Forward declarations for internal helper functions.
void initialize_gl_data(CPLPlot *plot);
void setup_plot_box_shaders(CPLPlot *plot);
void setup_grid_shaders(CPLPlot *plot);
void build_plot_box_data(CPLPlot *plot);
void build_grid_data(CPLPlot *plot, size_t rows, size_t cols);
void build_subplots_box_data(CPLFigure *fig, size_t rows, size_t cols,
                                    float margin);
void setup_plot_line_shaders(CPLPlot *plot, CPLLine *line);
void build_plot_line_data(CPLPlot *plot, double *x_arr, double *y_arr,
                                 size_t num_points, Color line_color,
                                 ColorCallback color_fn, void *user_data);
void build_plot_curve_data(CPLPlot *plot, double *t_arr, double *x_arr,
                                  double *y_arr, size_t num_points,
                                  Color line_color, ColorCallback color_fn,
                                  void *user_data);

#endif // CPL_GL_PLOT_H