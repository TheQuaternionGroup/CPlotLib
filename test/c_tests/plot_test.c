#include "CPlotLib.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// Function to dynamically assign colors based on a callback (optional)
Color dynamic_color_callback(double x, void* user_data) {
    // Example: Alternate colors based on the x value
    if (x < M_PI) {
        return (Color){1.0f, 0.0f, 0.0f}; // Red
    } else {
        return (Color){0.0f, 0.0f, 1.0f}; // Blue
    }
}

int main()
{
    // Create a figure with specified width and height
    CPLFigure* fig = CreateFigure(800, 600);
    if (!fig) {
        fprintf(stderr, "Failed to create figure.\n");
        return EXIT_FAILURE;
    }

    // Add a single plot to the figure
    CPLPlot* plot = AddPlot(fig);
    if (!plot) {
        fprintf(stderr, "Failed to add plot to figure.\n");
        FreeFigure(fig); // Assuming you have a function to free the figure
        return EXIT_FAILURE;
    }


    // Set X and Y ranges
    double x_range[2] = { 0.0, 2.0 * M_PI };
    double y_range[2] = { -2.0, 2.0 };
    SetXRange(plot, x_range);  // Set x-axis range
    SetYRange(plot, y_range);  // Set y-axis range
    // SetZRange(plot, z_range); // Optional if using 3D plots

    ShowGrid(plot, false); // Show grid lines
    // Prepare data for sin(x)
    size_t num_points = 1001; // Increased for smoother curves
    double x_sin[1001];
    double y_sin[1001];

    for (size_t i = 0; i < num_points; i++)  // Initialize all 1001 elements
    {
        double t = (2.0 * M_PI) * ((double)i / (double)(num_points - 1)); // Map i to [0, 2π]
        x_sin[i] = t;
        y_sin[i] = sin(x_sin[i]);
    }

    // Prepare data for cos(x)
    double x_cos[1001];
    double y_cos[1001];

    for (size_t i = 0; i < num_points; i++)  // Initialize all 1001 elements
    {
        double t = (2.0 * M_PI) * ((double)i / (double)(num_points - 1)); // Map i to [0, 2π]
        x_cos[i] = t;
        y_cos[i] = cos(x_cos[i]);
    }

    // Prepare data for sin(4x) to reduce jagged edges
    double x_sin4[1001];
    double y_sin4[1001];

    for (size_t i = 0; i < num_points; i++)  // Initialize all 1001 elements
    {
        double t = (2.0 * M_PI) * ((double)i / (double)(num_points - 1)); // Map i to [0, 2π]
        x_sin4[i] = t;
        y_sin4[i] = sin(4.0 * x_sin4[i]);
    }

    // Plot sin(x) in red
    Plot(plot, x_sin, y_sin, num_points, COLOR_RED, NULL, NULL);

    // Plot cos(x) in blue
    Plot(plot, x_cos, y_cos, num_points, COLOR_BLUE, NULL, NULL);

    // Plot sin(4x) in green for smoother curves
    Plot(plot, x_sin4, y_sin4, num_points, COLOR_GREEN, NULL, NULL);

    // Optionally, add another line with dynamic color
    Plot(plot, x_sin4, y_sin4, num_points, COLOR_BLACK, dynamic_color_callback, NULL);

    // Show the figure
    ShowFigure(fig);

    return EXIT_SUCCESS;
}