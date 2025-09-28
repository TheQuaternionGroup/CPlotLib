#include "CPLPlot.h"
#include "utils/CPLRenderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Internal function declarations
static CPLPlotData* cpl_create_plot_data(void);
static void cpl_free_plot_data(CPLPlotData* data);
static CPLSubplotLayout* cpl_create_subplot_layout(size_t rows, size_t cols, size_t index);
static void cpl_free_subplot_layout(CPLSubplotLayout* layout);
static void cpl_calculate_subplot_viewport(CPLSubplotLayout* layout, size_t rows, size_t cols, size_t index);
static void cpl_calculate_subplot_viewport_with_grid(CPLSubplotLayout* layout, size_t rows, size_t cols, size_t index, float grid_x_start, float grid_width);
static void cpl_build_subplot_viewports(CPLFigure* fig, size_t rows, size_t cols);
static void cpl_plot_error(const char* message);

// Constants
#define CPL_DEFAULT_MARGIN 0.1f
#define CPL_INITIAL_CAPACITY 4
#define CPL_MAX_STRING_LENGTH 63

// Plot management
CPLPlot* cpl_add_plot(CPLFigure* fig) {
    if (!fig) {
        cpl_plot_error("Invalid figure");
        return NULL;
    }

    // Expand array if needed
    if (fig->num_plots >= fig->capacity) {
        size_t new_capacity = fig->capacity == 0 ? CPL_INITIAL_CAPACITY : fig->capacity * 2;
        CPLPlot** new_plots = (CPLPlot**)realloc(fig->plots, new_capacity * sizeof(CPLPlot*));
        if (!new_plots) {
            cpl_plot_error("Failed to allocate memory for plots");
            return NULL;
        }
        fig->plots = new_plots;
        fig->capacity = new_capacity;
    }

    // Create new plot
    CPLPlot* plot = (CPLPlot*)calloc(1, sizeof(CPLPlot));
    if (!plot) {
        cpl_plot_error("Failed to allocate memory for plot");
        return NULL;
    }

    // Initialize plot
    plot->figure = fig;
    plot->x_range[0] = 0.0;
    plot->x_range[1] = 1.0;
    plot->y_range[0] = 0.0;
    plot->y_range[1] = 1.0;
    plot->show_grid = true;
    plot->show_axes = true;
    plot->show_ticks = true;
    
    // Initialize line thickness properties
    plot->line_width = 2.0f;        // Default line thickness
    plot->grid_line_width = 1.0f;   // Default grid line thickness
    plot->box_line_width = 2.0f;    // Default box line thickness
    
    plot->bg_color = COLOR_WHITE;
    plot->data = cpl_create_plot_data();
    plot->subplot_layout = NULL;
    plot->is_subplot = false;

    if (!plot->data) {
        free(plot);
        cpl_plot_error("Failed to create plot data");
        return NULL;
    }

    // Add to figure
    fig->plots[fig->num_plots] = plot;
    fig->num_plots++;

    return plot;
}

void cpl_add_subplots(CPLFigure* fig, size_t rows, size_t cols) {
    if (!fig || rows == 0 || cols == 0) {
        cpl_plot_error("Invalid figure or subplot dimensions");
        return;
    }

    size_t total_plots = rows * cols;
    
    // Ensure we have enough capacity for new subplots
    size_t required_capacity = fig->num_plots + total_plots;
    if (required_capacity > fig->capacity) {
        size_t new_capacity = required_capacity * 2; // Allocate extra space
        CPLPlot **new_plots = (CPLPlot **)realloc(fig->plots, new_capacity * sizeof(CPLPlot *));
        if (!new_plots) {
            cpl_plot_error("Could not allocate memory for subplot array");
            return;
        }
        fig->plots = new_plots;
        fig->capacity = new_capacity;
    }

    // Create each subplot
    for (size_t i = 0; i < total_plots; i++) {
        CPLPlot* plot = cpl_add_plot(fig);
        if (!plot) {
            cpl_plot_error("Failed to create subplot");
            return;
        }
        
        // Mark as subplot
        plot->is_subplot = true;
        
        // Set up subplot layout - this will be calculated in the viewport function
        plot->subplot_layout = cpl_create_subplot_layout(rows, cols, i);
        if (plot->subplot_layout) {
            plot->subplot_layout->margin = 0.1f; // 10% margin between subplots
        }
    }
    
    // Build subplot viewport data
    cpl_build_subplot_viewports(fig, rows, cols);
}

CPLPlot* cpl_get_subplot(CPLFigure* fig, size_t index) {
    if (!fig || !fig->plots || index >= fig->num_plots) {
        return NULL;
    }
    return fig->plots[index];
}

CPLPlot* cpl_get_subplot_at(CPLFigure* fig, size_t row, size_t col, size_t rows, size_t cols) {
    if (!fig || row >= rows || col >= cols) {
        return NULL;
    }
    size_t index = row * cols + col;
    return cpl_get_subplot(fig, index);
}

void cpl_free_plot(CPLPlot* plot) {
    if (!plot) return;

    if (plot->data) {
        cpl_free_plot_data(plot->data);
    }

    if (plot->subplot_layout) {
        cpl_free_subplot_layout(plot->subplot_layout);
    }

    free(plot);
}

// Subplot layout management
void cpl_set_subplot_layout(CPLPlot* plot, size_t rows, size_t cols, size_t index) {
    if (!plot) {
        cpl_plot_error("Invalid plot");
        return;
    }
    
    if (plot->subplot_layout) {
        cpl_free_subplot_layout(plot->subplot_layout);
    }
    
    plot->subplot_layout = cpl_create_subplot_layout(rows, cols, index);
    if (plot->subplot_layout) {
        cpl_calculate_subplot_viewport(plot->subplot_layout, rows, cols, index);
    }
}

void cpl_set_subplot_layout_with_grid(CPLPlot* plot, size_t rows, size_t cols, size_t index, float grid_x_start, float grid_width) {
    if (!plot) {
        cpl_plot_error("Invalid plot");
        return;
    }
    
    if (plot->subplot_layout) {
        cpl_free_subplot_layout(plot->subplot_layout);
    }
    
    plot->subplot_layout = cpl_create_subplot_layout(rows, cols, index);
    if (plot->subplot_layout) {
        cpl_calculate_subplot_viewport_with_grid(plot->subplot_layout, rows, cols, index, grid_x_start, grid_width);
    }
}

void cpl_set_subplot_margin(CPLPlot* plot, float margin) {
    if (!plot || !plot->subplot_layout) {
        cpl_plot_error("Invalid plot or subplot layout");
        return;
    }
    
    plot->subplot_layout->margin = margin;
}

void cpl_update_subplot_viewports(CPLFigure* fig) {
    if (!fig) return;
    
    // This function can be used to recalculate viewports if needed
    // For now, it's a placeholder
}

// Internal helper functions
static CPLPlotData* cpl_create_plot_data(void) {
    CPLPlotData* data = (CPLPlotData*)calloc(1, sizeof(CPLPlotData));
    if (!data) return NULL;
    
    data->lines = NULL;
    data->num_lines = 0;
    data->capacity = 0;
    data->box_vbo = 0;
    data->box_vao = 0;
    data->grid_vbo = 0;
    data->grid_vao = 0;
    data->grid_loaded = false;
    data->box_loaded = false;
    data->margin = CPL_DEFAULT_MARGIN;
    
    return data;
}

static void cpl_free_plot_data(CPLPlotData* data) {
    if (!data) return;
    
    // Free lines
    if (data->lines) {
        for (size_t i = 0; i < data->num_lines; i++) {
            if (data->lines[i].vertices) {
                free(data->lines[i].vertices);
            }
            if (data->lines[i].vbo) {
                glDeleteBuffers(1, &data->lines[i].vbo);
            }
            if (data->lines[i].vao) {
                glDeleteVertexArrays(1, &data->lines[i].vao);
            }
        }
        free(data->lines);
    }
    
    // Free OpenGL objects
    if (data->box_vbo) glDeleteBuffers(1, &data->box_vbo);
    if (data->box_vao) glDeleteVertexArrays(1, &data->box_vao);
    if (data->grid_vbo) glDeleteBuffers(1, &data->grid_vbo);
    if (data->grid_vao) glDeleteVertexArrays(1, &data->grid_vao);
    
    free(data);
}

static CPLSubplotLayout* cpl_create_subplot_layout(size_t rows, size_t cols, size_t index) {
    CPLSubplotLayout* layout = (CPLSubplotLayout*)calloc(1, sizeof(CPLSubplotLayout));
    if (!layout) return NULL;
    
    layout->rows = rows;
    layout->cols = cols;
    layout->index = index;
    layout->margin = CPL_DEFAULT_MARGIN;
    layout->left = 0.0f;
    layout->right = 1.0f;
    layout->bottom = 0.0f;
    layout->top = 1.0f;
    
    return layout;
}

static void cpl_free_subplot_layout(CPLSubplotLayout* layout) {
    free(layout);
}

static void cpl_calculate_subplot_viewport(CPLSubplotLayout* layout, size_t rows, size_t cols, size_t index) {
    if (!layout) return;
    
    size_t row = index / cols;
    size_t col = index % cols;
    
    float cell_width = 1.0f / cols;
    float cell_height = 1.0f / rows;
    
    layout->left = col * cell_width + layout->margin;
    layout->right = (col + 1) * cell_width - layout->margin;
    layout->bottom = 1.0f - (row + 1) * cell_height + layout->margin;
    layout->top = 1.0f - row * cell_height - layout->margin;
    
    // Ensure valid coordinates
    if (layout->left >= layout->right) {
        layout->left = col * cell_width;
        layout->right = (col + 1) * cell_width;
    }
    if (layout->bottom >= layout->top) {
        layout->bottom = 1.0f - (row + 1) * cell_height;
        layout->top = 1.0f - row * cell_height;
    }
}

static void cpl_calculate_subplot_viewport_with_grid(CPLSubplotLayout* layout, size_t rows, size_t cols, size_t index, float grid_x_start, float grid_width) {
    if (!layout) return;
    
    // Calculate row and column for this subplot within the grid
    size_t row = index / cols;
    size_t col = index % cols;
    
    // Calculate viewport coordinates within the grid
    float cell_width = grid_width / cols;
    float cell_height = 1.0f / rows;
    
    layout->left = grid_x_start + col * cell_width + layout->margin;
    layout->right = grid_x_start + (col + 1) * cell_width - layout->margin;
    layout->bottom = 1.0f - (row + 1) * cell_height + layout->margin;
    layout->top = 1.0f - row * cell_height - layout->margin;
    
    // Ensure valid coordinates
    if (layout->left >= layout->right) {
        layout->left = grid_x_start + col * cell_width;
        layout->right = grid_x_start + (col + 1) * cell_width;
    }
    if (layout->bottom >= layout->top) {
        layout->bottom = 1.0f - (row + 1) * cell_height;
        layout->top = 1.0f - row * cell_height;
    }
}

static void cpl_build_subplot_viewports(CPLFigure* fig, size_t rows, size_t cols) {
    if (!fig) return;
    
    // Find the start of the subplot grid (the last created plots)
    size_t subplot_start = fig->num_plots - (rows * cols);
    
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            size_t index = row * cols + col;
            size_t plot_index = subplot_start + index;
            
            if (plot_index >= fig->num_plots) {
                cpl_plot_error("Plot index out of range");
                continue;
            }
            
            CPLPlot* plot = fig->plots[plot_index];
            if (!plot || !plot->is_subplot || !plot->subplot_layout) {
                continue;
            }
            
            // Calculate viewport coordinates in normalized device coordinates (-1 to 1)
            float margin = plot->subplot_layout->margin;
            float total_margin_x = margin * (cols + 1);
            float total_margin_y = margin * (rows + 1);
            float subplot_width = (2.0f - total_margin_x) / (float)cols;
            float subplot_height = (2.0f - total_margin_y) / (float)rows;
            
            // Convert to screen coordinates (0 to 1)
            float left_ndc = -1.0f + margin + col * (subplot_width + margin);
            float right_ndc = left_ndc + subplot_width;
            float bottom_ndc = -1.0f + margin + row * (subplot_height + margin);
            float top_ndc = bottom_ndc + subplot_height;
            
            // Convert NDC to screen coordinates
            plot->subplot_layout->left = (left_ndc + 1.0f) / 2.0f;
            plot->subplot_layout->right = (right_ndc + 1.0f) / 2.0f;
            plot->subplot_layout->bottom = (bottom_ndc + 1.0f) / 2.0f;
            plot->subplot_layout->top = (top_ndc + 1.0f) / 2.0f;
        }
    }
}

static void cpl_plot_error(const char* message) {
    fprintf(stderr, "CPlotLib Error: %s\n", message);
}
