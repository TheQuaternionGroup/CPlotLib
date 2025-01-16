# CPlotLib

## Overview
**CPlotLib** is a small C/C++ plotting library using OpenGL, GLEW, and GLFW.  
You can create one or more plots within a window to quickly visualize your data in 2D.

### Building the Library
- Git clone repo into project or add as a submodule
- Run `make` in the root folder to build `lib/libcpl.a`.
- To clean up, run `make clean`.

### Linking and Compiling
After building, link against `libcpl.a` and include the headers:
```bash
gcc  my_code.c -Llib -lcpl -lGL -lGLEW -lglfw -lm -o my_program
```

## Basic C Example
Below is a **minimal** C snippet illustrating usage. It creates a plot of `sin(x)`:

```c
#include "CPlotLib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Create a figure (width=800, height=600)
    CPLFigure* fig = CreateFigure(800, 600);
    if (!fig) {
        fprintf(stderr, "Failed to create figure.\n");
        return EXIT_FAILURE;
    }

    // Add a single plot to the figure
    CPLPlot* plot = AddPlot(fig);
    if (!plot) {
        fprintf(stderr, "Failed to add plot.\n");
        FreeFigure(fig);
        return EXIT_FAILURE;
    }

    // Set axis ranges
    double x_range[2] = {0.0, 2.0 * M_PI};
    double y_range[2] = {-2.0, 2.0};
    SetXRange(plot, x_range);
    SetYRange(plot, y_range);

    // Prepare data for sin(x)
    size_t num_points = 1001;
    double x_data[1001], y_data[1001];
    for (size_t i = 0; i < num_points; i++) {
        double t = (2.0 * M_PI) * ((double)i / (double)(num_points - 1));
        x_data[i] = t;
        y_data[i] = sin(t);
    }

    // Plot in red
    Plot(plot, x_data, y_data, num_points, COLOR_RED, NULL, NULL);

    // Display window
    ShowFigure(fig);

    return EXIT_SUCCESS;
}
```

---

## Basic C++ Example
In C++, the usage is similar, but you’ll typically use the `cpl::Figure` and `cpl::Plot` classes:

```cpp
#include "CPlotLib.hpp"
#include <vector>
#include <cmath>
#include <iostream>

int main() {
    try {
        // Create a figure
        cpl::Figure fig(800, 600);

        // Add a single plot
        auto plot = fig.addPlot();

        // Set axis ranges
        plot->setXRange(0.0, 5.0);
        plot->setYRange(0.0, 50.0);

        // Sample data
        std::vector<double> x = {0, 1, 2, 3, 4, 5};
        std::vector<double> y = {0, 10, 20, 30, 40, 50};

        // Plot the data in red
        plot->plot(x, y, COLOR_RED);

        // Display figure window
        fig.show();
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
```

Compile and link similarly:
```bash
g++ -Iinclude my_code.cpp -Llib -lcpl -lGL -lGLEW -lglfw -lm -o my_program
```

---

## TODO
- **3D Plotting**: Extend library support for 3D charts.
- **Event Handling**: Add mouse/keyboard interactions (zoom, pan, etc.).
- **Export**: Implement functions to export plots as images (PNG, JPEG).
- **Legend and Labels**: Include customizable legends and axis labels.
- **Animations**: Enable real-time or animated updates for dynamic data.

Feel free to suggest or contribute more features!  
**Happy plotting!**
