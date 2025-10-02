#ifndef CPL_PLOT_H
#define CPL_PLOT_H

#include "CPLColors.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

// Forward declarations
struct CPLFigure;
struct CPLRenderer;

// Internal structures
typedef struct CPLLine {
    unsigned int vbo, vao;
    size_t num_vertices;
    float* vertices;
    bool is_loaded;
} CPLLine;

typedef struct CPLPlotData {
    CPLLine* lines;
    size_t num_lines;
    size_t capacity;
    
    // OpenGL objects for plot box and grid
    unsigned int box_vbo, box_vao;
    unsigned int grid_vbo, grid_vao;
    bool grid_loaded;
    bool box_loaded;
    
    float margin;
} CPLPlotData;

// Constants
#define CPL_DEFAULT_MARGIN 0.1f
#define CPL_INITIAL_CAPACITY 4
#define CPL_MAX_STRING_LENGTH 63

// Function pointer type for color callbacks
typedef Color (*CPLColorCallback)(double t, void* user_data);

// Subplot layout structure
typedef struct CPLSubplotLayout {
    size_t rows;                 // Number of rows
    size_t cols;                 // Number of columns
    size_t index;                // Subplot index (0-based)
    float left, right, bottom, top;  // Viewport coordinates [0,1]
    float margin;                // Margin around subplot
} CPLSubplotLayout;

// Plot structure
typedef struct CPLPlot {
    struct CPLFigure* figure;    // Parent figure
    struct CPLPlotData* data;    // Internal plot data
    
    // Plot dimensions and ranges
    double x_range[2];           // X-axis range [min, max]
    double y_range[2];           // Y-axis range [min, max]
    
    // Plot properties
    char title[64];              // Plot title
    char x_label[64];            // X-axis label
    char y_label[64];            // Y-axis label
    
    // Display options
    bool show_grid;              // Show grid lines
    bool show_axes;              // Show axes
    bool show_ticks;             // Show tick marks
    
    // Line properties
    float line_width;            // Line thickness in pixels
    float grid_line_width;       // Grid line thickness in pixels
    float box_line_width;        // Plot box line thickness in pixels
    
    Color bg_color;              // Background color
    
    // Subplot layout
    CPLSubplotLayout* subplot_layout;  // Subplot positioning info
    bool is_subplot;             // Whether this is part of a subplot grid
} CPLPlot;

// Figure structure
typedef struct CPLFigure {
    struct CPLRenderer* renderer; // OpenGL renderer
    CPLPlot** plots;             // Array of plots
    size_t num_plots;            // Number of plots
    size_t capacity;             // Current capacity
    
    size_t width;                // Figure width
    size_t height;               // Figure height
    Color bg_color;              // Background color
} CPLFigure;

// Core API functions
CPLFigure* cpl_create_figure(size_t width, size_t height);
void cpl_show_figure(CPLFigure* fig);
void cpl_free_figure(CPLFigure* fig);
void cpl_save_figure(CPLFigure* fig, const char* filename);

// Plot management
CPLPlot* cpl_add_plot(CPLFigure* fig);
void cpl_add_subplots(CPLFigure* fig, size_t rows, size_t cols);
CPLPlot* cpl_get_subplot(CPLFigure* fig, size_t index);
CPLPlot* cpl_get_subplot_at(CPLFigure* fig, size_t row, size_t col, size_t rows, size_t cols);

// Subplot layout management
void cpl_set_subplot_layout(CPLPlot* plot, size_t rows, size_t cols, size_t index);
void cpl_set_subplot_layout_with_grid(CPLPlot* plot, size_t rows, size_t cols, size_t index, float grid_x_start, float grid_width);
void cpl_set_subplot_margin(CPLPlot* plot, float margin);
void cpl_update_subplot_viewports(CPLFigure* fig);

// Plot configuration
void cpl_set_x_range(CPLPlot* plot, double min, double max);
void cpl_set_y_range(CPLPlot* plot, double min, double max);
void cpl_set_title(CPLPlot* plot, const char* title);
void cpl_set_x_label(CPLPlot* plot, const char* label);
void cpl_set_y_label(CPLPlot* plot, const char* label);
void cpl_show_grid(CPLPlot* plot, bool show);
void cpl_show_axes(CPLPlot* plot, bool show);
void cpl_set_background_color(CPLPlot* plot, Color color);

// Line thickness control
void cpl_set_line_width(CPLPlot* plot, float width);
void cpl_set_grid_line_width(CPLPlot* plot, float width);
void cpl_set_box_line_width(CPLPlot* plot, float width);

// Data plotting
void cpl_plot(CPLPlot* plot, const double* x, const double* y, size_t n_points, Color color, CPLColorCallback color_fn, void* user_data);
void cpl_plot_parametric(CPLPlot* plot, const double* t, const double* x,  const double* y, size_t n_points, Color color, CPLColorCallback color_fn, void* user_data);

// Plot rendering
void cpl_render_plot(CPLPlot* plot);

#ifdef __cplusplus
}
#endif

#endif // CPL_PLOT_H
