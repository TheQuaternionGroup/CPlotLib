#include "CPlotLib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("CPlotLib Test (C)\n");
    printf("================\n\n");

    // Test 1: Basic figure creation
    printf("Test 1: Creating figure...\n");
    CPLFigure* fig = cpl_create_figure(800, 600);
    if (!fig) {
        fprintf(stderr, "FAILED: Could not create figure\n");
        return EXIT_FAILURE;
    }
    printf("✓ Figure created successfully\n");

    // Test 2: Adding plot
    printf("Test 2: Adding plot...\n");
    CPLPlot* plot = cpl_add_plot(fig);
    if (!plot) {
        fprintf(stderr, "FAILED: Could not add plot\n");
        cpl_free_figure(fig);
        return EXIT_FAILURE;
    }
    printf("✓ Plot added successfully\n");

    // Test 3: Setting ranges
    printf("Test 3: Setting ranges...\n");
    cpl_set_x_range(plot, 0.0, 2.0 * M_PI);
    cpl_set_y_range(plot, -1.5, 1.5);
    printf("✓ Ranges set successfully\n");

    // Test 4: Setting properties
    printf("Test 4: Setting plot properties...\n");
    cpl_set_title(plot, "Test Plot");
    cpl_set_x_label(plot, "X");
    cpl_set_y_label(plot, "Y");
    cpl_show_grid(plot, true);
    printf("✓ Properties set successfully\n");

    // Test 5: Generating and plotting data
    printf("Test 5: Generating and plotting data...\n");
    const size_t num_points = 100;
    double x[num_points];
    double y[num_points];

    for (size_t i = 0; i < num_points; i++) {
        x[i] = 2.0 * M_PI * ((double)i / (num_points - 1));
        y[i] = sin(x[i]);
    }

    cpl_plot(plot, x, y, num_points, COLOR_RED, NULL, NULL);
    printf("✓ Data plotted successfully\n");

    printf("\nAll tests passed! 🎉\n");
    printf("The plot window should open. Press ESC to close.\n\n");

    // Show the figure
    cpl_show_figure(fig);

    return EXIT_SUCCESS;
}
