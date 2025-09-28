#include "CPlotLib.hpp"
#include <iostream>
#include <vector>
#include <cmath>

int main() {
    std::cout << "CPlotLib Subplot Example (C++)" << std::endl;
    std::cout << "==============================" << std::endl;
    
    try {
        // Create a figure
        cpl::Figure fig(800, 600);
        
        // Create a 2x2 subplot grid
        auto subplots = fig.addSubplots(2, 2);
        
        // Generate data for different plots
        const size_t n_points = 100;
        std::vector<double> x(n_points);
        std::vector<double> y1(n_points);
        std::vector<double> y2(n_points);
        std::vector<double> y3(n_points);
        std::vector<double> y4(n_points);
        
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
        if (subplots.size() > 0) {
            auto& plot1 = subplots[0];
            plot1->setXRange(0, 4 * M_PI);
            plot1->setYRange(-1.2, 1.2);
            plot1->setTitle("Sine Wave");
            plot1->setXLabel("t");
            plot1->setYLabel("sin(t)");
            plot1->showGrid(true);
            
            Color red = {1.0f, 0.0f, 0.0f, 1.0f};
            plot1->plot(x, y1, red);
        }
        
        // Configure subplot 2 (top-right): Cosine wave
        if (subplots.size() > 1) {
            auto& plot2 = subplots[1];
            plot2->setXRange(0, 4 * M_PI);
            plot2->setYRange(-1.2, 1.2);
            plot2->setTitle("Cosine Wave");
            plot2->setXLabel("t");
            plot2->setYLabel("cos(t)");
            plot2->showGrid(true);
            
            Color green = {0.0f, 1.0f, 0.0f, 1.0f};
            plot2->plot(x, y2, green);
        }
        
        // Configure subplot 3 (bottom-left): Product
        if (subplots.size() > 2) {
            auto& plot3 = subplots[2];
            plot3->setXRange(0, 4 * M_PI);
            plot3->setYRange(-1.2, 1.2);
            plot3->setTitle("sin(t) * cos(t)");
            plot3->setXLabel("t");
            plot3->setYLabel("sin(t) * cos(t)");
            plot3->showGrid(true);
            
            Color blue = {0.0f, 0.0f, 1.0f, 1.0f};
            plot3->plot(x, y3, blue);
        }
        
        // Configure subplot 4 (bottom-right): Sum
        if (subplots.size() > 3) {
            auto& plot4 = subplots[3];
            plot4->setXRange(0, 4 * M_PI);
            plot4->setYRange(-1.5, 1.5);
            plot4->setTitle("sin(t) + 0.5*sin(3t)");
            plot4->setXLabel("t");
            plot4->setYLabel("sin(t) + 0.5*sin(3t)");
            plot4->showGrid(true);
            
            Color yellow = {1.0f, 1.0f, 0.0f, 1.0f};
            plot4->plot(x, y4, yellow);
        }
        
        std::cout << "Subplots created successfully!" << std::endl;
        std::cout << "Press ESC to close the window." << std::endl;
        
        // Show the figure
        fig.show();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
