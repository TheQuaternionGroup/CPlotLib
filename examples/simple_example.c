#include "CPlotLib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("CPlotLib Simple Example (Refactored)\n");
    printf("===================================\n\n");

    // Create a figure
    CPLFigure* fig = cpl_create_figure(800, 600);
    if (!fig) {
        fprintf(stderr, "Failed to create figure\n");
        return EXIT_FAILURE;
    }

    // Add a plot
    CPLPlot* plot = cpl_add_plot(fig);
    if (!plot) {
        fprintf(stderr, "Failed to add plot\n");
        cpl_free_figure(fig);
        return EXIT_FAILURE;
    }

    // Set axis ranges
    cpl_set_x_range(plot, 0.0, 2.0 * M_PI);
    cpl_set_y_range(plot, -1.5, 1.5);

    // Set plot properties
    cpl_set_title(plot, "Sine and Cosine Waves");
    cpl_set_x_label(plot, "X (radians)");
    cpl_set_y_label(plot, "Y (amplitude)");
    cpl_show_grid(plot, true);

    // Generate data for sine and cosine waves
    const size_t num_points = 200;
    double x[num_points];
    double y_sin[num_points];
    double y_cos[num_points];

    for (size_t i = 0; i < num_points; i++) {
        x[i] = 2.0 * M_PI * ((double)i / (num_points - 1));
        y_sin[i] = sin(x[i]);
        y_cos[i] = cos(x[i]);
    }

    // Plot the data
    cpl_plot(plot, x, y_sin, num_points, COLOR_RED, NULL, NULL);
    cpl_plot(plot, x, y_cos, num_points, COLOR_BLUE, NULL, NULL);

    printf("Plot created successfully!\n");
    printf("Press ESC to close the window.\n\n");

    // Show the figure (this will block until window is closed)
    cpl_show_figure(fig);

    return EXIT_SUCCESS;
}
