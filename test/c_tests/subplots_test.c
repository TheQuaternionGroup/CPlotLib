#include "CPlotLib.h"

#include <stdio.h>
#include <math.h>

#define POINTS 1000
#define PI 3.14159265358979323846

// Define colors
const Color BLUE = COLOR_BLUE;
const Color RED = COLOR_RED;
const Color GREEN = COLOR_GREEN;
const Color ORANGE = (Color){1.0f, 0.5f, 0.0f, 1.0f};

// Typedef for mathematical functions
typedef double (*MathFunc)(double);

// Example color callback
Color gradient_color(double x, void* userData) {
    Color base = *((Color*)userData);
    return (Color){base.r * (x / 10.0), base.g * (x / 10.0), base.b * (x / 10.0), 1.0f};
}

// Exponential decay function
double exponential_decay(double x) {
    return x * x;
}

// Helper function to create and plot data
void create_and_plot(CPLFigure* fig, int subplot_index, double x_start, double x_end, double y_min, double y_max, MathFunc func, Color color, Color (*color_callback)(double, void*), void* userData) {
    CPLPlot* plot = fig->plot[subplot_index];
    SetXRange(plot, (double[]){x_start, x_end});
    SetYRange(plot, (double[]){y_min, y_max});
    // ShowGrid(plot, true);
    
    double x[POINTS];
    double y[POINTS];
    for (size_t i = 0; i < POINTS; i++) {
        x[i] = x_start + i * (x_end - x_start) / (POINTS - 1);
        y[i] = func(x[i]);
    }
    Plot(plot, x, y, POINTS, color, color_callback, userData);
}

double triangle_wave(double x) {
    return 2 * fabs(fmod(x, 1) - 0.5) - 1;
}

int main() {
    // Create a figure
    CPLFigure* fig = CreateFigure(800, 600);
    if (!fig) return -1;

    // Add a 2x2 grid of subplots
    AddSubplots(fig, 2, 2);

    // Plot sine wave
    create_and_plot(fig, 0, 0, 2 * PI, -1, 1, sin, BLUE, NULL, NULL);

    // Plot cosine wave
    create_and_plot(fig, 1, 0, 2 * PI, -1, 1, cos, RED, NULL, NULL);

    create_and_plot(fig, 1, 0, 2 * PI, -1, 1, triangle_wave, COLOR_VIOLET, NULL, NULL);

    // Plot tangent wave with gradient color
    Color baseColor = GREEN;
    create_and_plot(fig, 2, -PI / 2, PI / 2, -10, 10, tan, baseColor, gradient_color, &baseColor);

    // Plot exponential decay
    create_and_plot(fig, 3, 0, 5, 0, 1, exponential_decay, ORANGE, NULL, NULL);

    // Show the figure
    ShowFigure(fig);

    return 0;
}