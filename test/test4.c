#include "CPlotLib.h"

#include <math.h>


int main() {
    // Create a figure
    CPLFigure* fig = CreateFigure(800, 600);

    // Add a plot
    CPLPlot* plot = AddPlot(fig);

    // Set axis ranges
    double x_range[2] = {-10.0, 10.0};
    double y_range[2] = {-10.0, 10.0};
    double z_range[2] = {-10.0, 10.0};
    SetXRange(plot, x_range);
    SetYRange(plot, y_range);
    SetZRange(plot, z_range);

    // Define data points for 3D plot (e.g., a helix)
    size_t num_points = 1000;
    double x_arr[num_points];
    double y_arr[num_points];
    double z_arr[num_points];
    for (size_t i = 0; i < num_points; i++) {
        double theta = i * 0.1;
        x_arr[i] = 5.0 * cos(theta);
        y_arr[i] = 5.0 * sin(theta);
        z_arr[i] = theta * 0.1;
    }

    // Plot the 3D line
    Plot(plot, x_arr, y_arr, z_arr, num_points, COLOR_RED, NULL, NULL);

    // Display the figure
    ShowFigure(fig);

    return 0;
}
