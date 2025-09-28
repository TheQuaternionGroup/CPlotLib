#include "CPLPlot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Internal function declarations
static void cpl_plot_error(const char* message);

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
