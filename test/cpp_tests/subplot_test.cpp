#include "CPlotLib.hpp"

#include <vector>
#include <iostream>
#include <cmath>

#define POINTS 1000
#define PI 3.14159265358979323846

typedef double (*MathFunc)(double);

Color gradient_color(double x, void* userData) {
    Color base = *((Color*)userData);
    float factor = static_cast<float>(x / 10.0);
    return (Color){base.r * factor, base.g * factor, base.b * factor, 1.0f};
}

double triangle_wave(double x) {
    return 2 * fabs(fmod(x, 1.0) - 0.5) - 1;
}

double exponential_decay(double x) {
    return exp(-x);
}

// Helper to generate x,y data
static std::pair<std::vector<double>, std::vector<double>> generateData(
    double start, double end, size_t points, MathFunc func
) {
    std::vector<double> x(points), y(points);
    const double step = (end - start) / (points - 1);
    for (size_t i = 0; i < points; ++i) {
        x[i] = start + i * step;
        y[i] = func(x[i]);
    }
    return {x, y};
}

int main() {
    try {
        cpl::Figure fig(800, 600);
        auto subplots = fig.addSubplots(2, 2);

        const Color BLUE = COLOR_BLUE;
        const Color RED = COLOR_RED;
        const Color VIOLET = COLOR_VIOLET;
        const Color GREEN = COLOR_GREEN;
        const Color ORANGE = (Color){1.0f, 0.5f, 0.0f, 1.0f};

        // Subplot 0: sine wave
        {
            auto* plot = subplots[0];
            plot->setXRange(0.0, 2 * PI);
            plot->setYRange(-1.0, 1.0);
            plot->setBackgroundColor(COLOR_LIGHTGRAY);

            auto [x, y] = generateData(0.0, 2 * PI, POINTS, [](double v){ return sin(v); });
            plot->plot(x, y, BLUE);
        }

        // Subplot 1: cosine wave
        {
            auto* plot = subplots[1];
            plot->setXRange(0.0, 2 * PI);
            plot->setYRange(-1.0, 1.0);
            plot->setBackgroundColor(COLOR_LIGHTGRAY);

            auto [x, y] = generateData(0.0, 2 * PI, POINTS, [](double v){ return cos(v); });
            plot->plot(x, y, RED);
        }

        // Overlay triangle wave on subplot 1
        {
            auto* plot = subplots[1];
            auto [x, y] = generateData(0.0, 2 * PI, POINTS, triangle_wave);
            plot->plot(x, y, VIOLET);
        }

        // Subplot 2: tangent wave with gradient color
        {
            auto* plot = subplots[2];
            plot->setXRange(-PI / 2 + 0.1, PI / 2 - 0.1);
            plot->setYRange(-10.0, 10.0);
            plot->setBackgroundColor(COLOR_LIGHTGRAY);

            auto [x, y] = generateData(-PI / 2 + 0.1, PI / 2 - 0.1, POINTS, [](double v){ return tan(v); });
            Color baseColor = GREEN;
            plot->plot(x, y, baseColor, gradient_color, &baseColor);
        }

        // Subplot 3: exponential decay
        {
            auto* plot = subplots[3];
            plot->setXRange(0.0, 5.0);
            plot->setYRange(0.0, 1.0);
            plot->setBackgroundColor(COLOR_LIGHTGRAY);

            auto [x, y] = generateData(0.0, 5.0, POINTS, exponential_decay);
            plot->plot(x, y, ORANGE);
        }

        fig.show();
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}