#include "CPlotLib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    printf("CPlotLib Subplot Example\n");
    printf("========================\n");
    
    // Create a figure
    CPLFigure* fig = cpl_create_figure(800, 600);
    if (!fig) {
        printf("Failed to create figure\n");
        return 1;
    }
    
    // Create a 2x2 subplot grid
    cpl_add_subplots(fig, 2, 2);
    
    // Generate data for different plots
    const size_t n_points = 100;
    double* x = malloc(n_points * sizeof(double));
    double* y1 = malloc(n_points * sizeof(double));
    double* y2 = malloc(n_points * sizeof(double));
    double* y3 = malloc(n_points * sizeof(double));
    double* y4 = malloc(n_points * sizeof(double));
    
    if (!x || !y1 || !y2 || !y3 || !y4) {
        printf("Failed to allocate data arrays\n");
        return 1;
    }
    
    // Generate data
    for (size_t i = 0; i < n_points; i++) {
        double t = (double)i / (n_points - 1) * 4 * M_PI;
        x[i] = t;
        y1[i] = sin(t);                    // Sine wave
        y2[i] = cos(t);                    // Cosine wave
        y3[i] = sin(t) * cos(t);           // Product of sine and cosine
        y4[i] = sin(t) + 0.5 * sin(3 * t); // Sum of sine waves
    }
    
    // Configure subplot 1 (top-left): Sine wave
    CPLPlot* plot1 = cpl_get_subplot(fig, 0);
    if (plot1) {
        cpl_set_x_range(plot1, 0, 4 * M_PI);
        cpl_set_y_range(plot1, -1.2, 1.2);
        cpl_set_title(plot1, "Sine Wave");
        cpl_set_x_label(plot1, "t");
        cpl_set_y_label(plot1, "sin(t)");
        cpl_show_grid(plot1, true);
        
        Color red = {1.0f, 0.0f, 0.0f, 1.0f};
        cpl_plot(plot1, x, y1, n_points, red, NULL, NULL);
    }
    
    // Configure subplot 2 (top-right): Cosine wave
    CPLPlot* plot2 = cpl_get_subplot(fig, 1);
    if (plot2) {
        cpl_set_x_range(plot2, 0, 4 * M_PI);
        cpl_set_y_range(plot2, -1.2, 1.2);
        cpl_set_title(plot2, "Cosine Wave");
        cpl_set_x_label(plot2, "t");
        cpl_set_y_label(plot2, "cos(t)");
        cpl_show_grid(plot2, true);
        
        Color green = {0.0f, 1.0f, 0.0f, 1.0f};
        cpl_plot(plot2, x, y2, n_points, green, NULL, NULL);
    }
    
    // Configure subplot 3 (bottom-left): Product
    CPLPlot* plot3 = cpl_get_subplot(fig, 2);
    if (plot3) {
        cpl_set_x_range(plot3, 0, 4 * M_PI);
        cpl_set_y_range(plot3, -1.2, 1.2);
        cpl_set_title(plot3, "sin(t) * cos(t)");
        cpl_set_x_label(plot3, "t");
        cpl_set_y_label(plot3, "sin(t) * cos(t)");
        cpl_show_grid(plot3, true);
        
        Color blue = {0.0f, 0.0f, 1.0f, 1.0f};
        cpl_plot(plot3, x, y3, n_points, blue, NULL, NULL);
    }
    
    // Configure subplot 4 (bottom-right): Sum
    CPLPlot* plot4 = cpl_get_subplot(fig, 3);
    if (plot4) {
        cpl_set_x_range(plot4, 0, 4 * M_PI);
        cpl_set_y_range(plot4, -1.5, 1.5);
        cpl_set_title(plot4, "sin(t) + 0.5*sin(3t)");
        cpl_set_x_label(plot4, "t");
        cpl_set_y_label(plot4, "sin(t) + 0.5*sin(3t)");
        cpl_show_grid(plot4, true);
        
        Color yellow = {1.0f, 1.0f, 0.0f, 1.0f};
        cpl_plot(plot4, x, y4, n_points, yellow, NULL, NULL);
    }
    
    printf("Subplots created successfully!\n");
    printf("Press ESC to close the window.\n");
    
    // Show the figure
    cpl_show_figure(fig);
    
    // Cleanup
    free(x);
    free(y1);
    free(y2);
    free(y3);
    free(y4);
    
    return 0;
}
