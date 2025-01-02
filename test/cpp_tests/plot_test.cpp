#include "CPlotLib.hpp"

#include <vector>
#include <iostream>

int main() {
    try {
        // Create a figure with width=800 and height=600
        cpl::Figure fig(800, 600);

        // Add a plot to the figure
        auto plot = fig.addPlot();

        // Set axis ranges
        plot->setXRange(0.0, 5.0);
        plot->setYRange(0.0, 50.0);


        // Set background color to light gray
        plot->setBackgroundColor(COLOR_LIGHTGRAY);

        // Sample data
        std::vector<double> x = {0, 1, 2, 3, 4, 5};
        std::vector<double> y = {0, 10, 20, 30, 40, 50};

        // Plot the data with red color
        plot->plot(x, y, COLOR_RED);

        // plor x^2
        int n = 100;
        std::vector<double> x2(n), y2(n);
        for (int i = 0; i < n; i++) {
            x2[i] = i / 10.0;
            y2[i] = x2[i] * x2[i];
        }
        plot->plot(x2, y2, COLOR_BLUE);


        // Show the figure window
        fig.show();
    }
    catch (const std::exception& ex) {
        // Handle any errors
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
