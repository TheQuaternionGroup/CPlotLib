#include "CPLPlot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>

// Internal function declarations
static void cpl_plot_error(const char* message);

// External function declarations
void cpl_setup_grid(CPLPlot* plot);

// Plot configuration
void cpl_set_x_range(CPLPlot* plot, double min, double max) {
    if (!plot || min >= max) {
        cpl_plot_error("Invalid plot or range");
        return;
    }
    plot->x_range[0] = min;
    plot->x_range[1] = max;
}

void cpl_set_y_range(CPLPlot* plot, double min, double max) {
    if (!plot || min >= max) {
        cpl_plot_error("Invalid plot or range");
        return;
    }
    plot->y_range[0] = min;
    plot->y_range[1] = max;
}

void cpl_set_title(CPLPlot* plot, const char* title) {
    if (!plot || !title) return;
    strncpy(plot->title, title, CPL_MAX_STRING_LENGTH);
    plot->title[CPL_MAX_STRING_LENGTH] = '\0';
}

void cpl_set_x_label(CPLPlot* plot, const char* label) {
    if (!plot || !label) return;
    strncpy(plot->x_label, label, CPL_MAX_STRING_LENGTH);
    plot->x_label[CPL_MAX_STRING_LENGTH] = '\0';
}

void cpl_set_y_label(CPLPlot* plot, const char* label) {
    if (!plot || !label) return;
    strncpy(plot->y_label, label, CPL_MAX_STRING_LENGTH);
    plot->y_label[CPL_MAX_STRING_LENGTH] = '\0';
}

void cpl_show_grid(CPLPlot* plot, bool show) {
    if (!plot) return;
    plot->show_grid = show;
}

void cpl_show_axes(CPLPlot* plot, bool show) {
    if (!plot) return;
    plot->show_axes = show;
    
    // If grid is already loaded, regenerate it to update axis colors
    if (plot->data && plot->data->grid_loaded) {
        // Free existing grid OpenGL objects
        if (plot->data->grid_vbo) {
            glDeleteBuffers(1, &plot->data->grid_vbo);
            plot->data->grid_vbo = 0;
        }
        if (plot->data->grid_vao) {
            glDeleteVertexArrays(1, &plot->data->grid_vao);
            plot->data->grid_vao = 0;
        }
        plot->data->grid_loaded = false;
        
        // Regenerate grid with new axis settings
        if (plot->show_grid) {
            cpl_setup_grid(plot);
        }
    }
}

void cpl_set_background_color(CPLPlot* plot, Color color) {
    if (!plot) return;
    plot->bg_color = color;
}

// Line thickness control functions
void cpl_set_line_width(CPLPlot* plot, float width) {
    if (!plot) {
        cpl_plot_error("Invalid plot");
        return;
    }
    
    if (width <= 0.0f) {
        cpl_plot_error("Line width must be positive");
        return;
    }
    
    plot->line_width = width;
}

void cpl_set_grid_line_width(CPLPlot* plot, float width) {
    if (!plot) {
        cpl_plot_error("Invalid plot");
        return;
    }
    
    if (width <= 0.0f) {
        cpl_plot_error("Grid line width must be positive");
        return;
    }
    
    plot->grid_line_width = width;
}

void cpl_set_box_line_width(CPLPlot* plot, float width) {
    if (!plot) {
        cpl_plot_error("Invalid plot");
        return;
    }
    
    if (width <= 0.0f) {
        cpl_plot_error("Box line width must be positive");
        return;
    }
    
    plot->box_line_width = width;
}

// Internal helper functions
static void cpl_plot_error(const char* message) {
    fprintf(stderr, "CPlotLib Error: %s\n", message);
}
