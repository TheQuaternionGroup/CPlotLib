#include "CPLPlot.h"
#include "utils/CPLRenderer.h"

#include <stdio.h>
#include <stdlib.h>

// Internal function declarations
static void cpl_plot_error(const char* message);

// Forward declarations for functions in other modules
extern void cpl_free_plot(CPLPlot* plot);

// Core API implementation
CPLFigure* cpl_create_figure(size_t width, size_t height) {
    if (width == 0 || height == 0) {
        cpl_plot_error("Invalid figure dimensions");
        return NULL;
    }

    CPLFigure* fig = (CPLFigure*)calloc(1, sizeof(CPLFigure));
    if (!fig) {
        cpl_plot_error("Failed to allocate memory for figure");
        return NULL;
    }

    // Initialize renderer
    fig->renderer = cpl_create_renderer(width, height);
    if (!fig->renderer) {
        cpl_plot_error("Failed to create renderer");
        free(fig);
        return NULL;
    }

    // Initialize figure properties
    fig->plots = NULL;
    fig->num_plots = 0;
    fig->capacity = 0;
    fig->width = width;
    fig->height = height;
    fig->bg_color = COLOR_WHITE;

    return fig;
}

void cpl_show_figure(CPLFigure* fig) {
    if (!fig || !fig->renderer) {
        cpl_plot_error("Invalid figure or renderer");
        return;
    }
    
    cpl_run_render_loop(fig);
}

void cpl_free_figure(CPLFigure* fig) {
    if (!fig) return;

    // Free all plots
    if (fig->plots) {
        for (size_t i = 0; i < fig->num_plots; i++) {
            if (fig->plots[i]) {
                cpl_free_plot(fig->plots[i]);
            }
        }
        free(fig->plots);
    }

    // Free renderer
    if (fig->renderer) {
        cpl_destroy_renderer(fig->renderer);
    }

    free(fig);
}

void cpl_save_figure(CPLFigure* fig, const char* filename) {
    if (!fig || !filename) {
        cpl_plot_error("Invalid figure or filename");
        return;
    }
    
    // TODO: Implement actual file saving functionality
    printf("Save functionality not yet implemented: %s\n", filename);
}

// Internal helper functions
static void cpl_plot_error(const char* message) {
    fprintf(stderr, "CPlotLib Error: %s\n", message);
}
