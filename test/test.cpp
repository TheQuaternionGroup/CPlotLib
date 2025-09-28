#include "CPlotLib.hpp"
#include <cmath>
#include <iostream>
#include <vector>

int main() {
    std::cout << "CPlotLib Test (C++)" << std::endl;
    std::cout << "==================" << std::endl << std::endl;

    try {
        // Test 1: Basic figure creation
        std::cout << "Test 1: Creating figure..." << std::endl;
        cpl::Figure fig(800, 600);
        std::cout << "✓ Figure created successfully" << std::endl;

        // Test 2: Adding plot
        std::cout << "Test 2: Adding plot..." << std::endl;
        auto plot = fig.addPlot();
        std::cout << "✓ Plot added successfully" << std::endl;

        // Test 3: Setting ranges
        std::cout << "Test 3: Setting ranges..." << std::endl;
        plot->setXRange(0.0, 2.0 * M_PI);
        plot->setYRange(-1.5, 1.5);
        std::cout << "✓ Ranges set successfully" << std::endl;

        // Test 4: Setting properties
        std::cout << "Test 4: Setting plot properties..." << std::endl;
        plot->setTitle("Test Plot (C++)");
        plot->setXLabel("X");
        plot->setYLabel("Y");
        plot->showGrid(true);
        std::cout << "✓ Properties set successfully" << std::endl;

        // Test 5: Generating and plotting data
        std::cout << "Test 5: Generating and plotting data..." << std::endl;
        const size_t num_points = 100;
        std::vector<double> x(num_points);
        std::vector<double> y(num_points);

        for (size_t i = 0; i < num_points; ++i) {
            x[i] = 2.0 * M_PI * (static_cast<double>(i) / (num_points - 1));
            y[i] = std::sin(x[i]);
        }

        plot->plot(x, y, COLOR_BLUE);
        std::cout << "✓ Data plotted successfully" << std::endl;

        std::cout << std::endl << "All tests passed! 🎉" << std::endl;
        std::cout << "The plot window should open. Press ESC to close." << std::endl << std::endl;

        // Show the figure
        fig.show();

    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
