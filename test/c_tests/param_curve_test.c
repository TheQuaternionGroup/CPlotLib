// test/test1.c

#include "CPlotLib.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

static double f_x(double t) { return cos(t)*sin(4*t); }
static double f_y(double t) { return sin(t)*sin(4*t); }

static double df_x(double t) 
{
    return 4.0*cos(t)*cos(4*t) - sin(t)*sin(4*t);
}
static double df_y(double t)
{
    return cos(t)*sin(4*t) + 4.0*sin(t)*cos(4*t);
}
static double d2f_x(double t)
{
    return -17.0*cos(t)*sin(4*t) - 8.0*sin(t)*cos(4*t);
}
static double d2f_y(double t)
{
    return -17.0* sin(t)*sin(4*t) + 8.0*cos(t)*cos(4*t);
}
static double d3f_x(double t){ return 0.0; }
static double d3f_y(double t){ return 0.0; }


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
        FreeFigure(fig); 
        return EXIT_FAILURE;
    }

    // Set X and Y ranges
    double x_range[2] = { -2.0, 2.0 };
    double y_range[2] = { -2.0, 2.0 };
    SetXRange(plot, x_range);  
    SetYRange(plot, y_range);  

    // Prepare data
    size_t num_points = 300;
    double x[300];
    double y[300];

    for (size_t i = 0; i < num_points; i++)  // Changed to i < num_points to avoid out-of-bounds
    {
        double t = (2.0 * M_PI) * ((double)i / (double)(num_points - 1)); // Map i to [0, 2π]
        x[i] = f_x(t);
        y[i] = f_y(t);
    }

    // Plot the data
    Plot(plot, x, y, num_points, COLOR_RED, NULL, NULL);  

    // Show the figure
    ShowFigure(fig);

    return EXIT_SUCCESS;
}
