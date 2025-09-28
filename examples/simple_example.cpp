#include "CPlotLib.hpp"
#include <cmath>
#include <iostream>
#include <vector>

int main() {
    std::cout << "CPlotLib C++ Simple Example (Refactored)" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;

    try {
        // Create a figure
        cpl::Figure fig(800, 600);

        // Add a plot
        auto plot = fig.addPlot();

        // Set axis ranges
        plot->setXRange(0.0, 2.0 * M_PI);
        plot->setYRange(-1.5, 1.5);

        // Set plot properties
        plot->setTitle("Sine and Cosine Waves");
        plot->setXLabel("X (radians)");
        plot->setYLabel("Y (amplitude)");
        plot->showGrid(true);

        // Generate data for sine and cosine waves
        const size_t num_points = 200;
        std::vector<double> x(num_points);
        std::vector<double> y_sin(num_points);
        std::vector<double> y_cos(num_points);

        for (size_t i = 0; i < num_points; ++i) {
            x[i] = 2.0 * M_PI * (static_cast<double>(i) / (num_points - 1));
            y_sin[i] = std::sin(x[i]);
            y_cos[i] = std::cos(x[i]);
        }

        // Plot the data
        plot->plot(x, y_sin, COLOR_RED);
        plot->plot(x, y_cos, COLOR_BLUE);

        std::cout << "Plot created successfully!" << std::endl;
        std::cout << "Press ESC to close the window." << std::endl << std::endl;

        // Show the figure (this will block until window is closed)
        fig.show();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
