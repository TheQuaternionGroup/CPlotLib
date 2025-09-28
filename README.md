# CPlotLib v2.0

A modern, optimized C/C++ plotting library using OpenGL for high-performance 2D data visualization.

## Features

- **Clean API**: Simplified, consistent function naming and structure
- **High Performance**: OpenGL-based rendering for smooth, fast plotting
- **Dual Language Support**: Both C and C++ interfaces
- **Modern C++**: RAII, move semantics, and exception safety
- **Cross-Platform**: Works on macOS, Linux, and Windows
- **Minimal Dependencies**: Only OpenGL, GLEW, and GLFW

## Quick Start

### C Example

```c
#include "CPlotLib.h"
#include <math.h>

int main() {
    // Create figure and plot
    CPLFigure* fig = cpl_create_figure(800, 600);
    CPLPlot* plot = cpl_add_plot(fig);
    
    // Set ranges and plot data
    cpl_set_x_range(plot, 0.0, 2.0 * M_PI);
    cpl_set_y_range(plot, -1.5, 1.5);
    
    // Generate and plot sine wave
    double x[100], y[100];
    for (int i = 0; i < 100; i++) {
        x[i] = 2.0 * M_PI * i / 99.0;
        y[i] = sin(x[i]);
    }
    cpl_plot(plot, x, y, 100, COLOR_RED, NULL, NULL);
    
    // Display
    cpl_show_figure(fig);
    return 0;
}
```

### C++ Example

```cpp
#include "CPlotLib.hpp"
#include <vector>
#include <cmath>

int main() {
    try {
        // Create figure and plot
        cpl::Figure fig(800, 600);
        auto plot = fig.addPlot();
        
        // Set ranges and properties
        plot->setXRange(0.0, 2.0 * M_PI);
        plot->setYRange(-1.5, 1.5);
        plot->setTitle("Sine Wave");
        plot->showGrid(true);
        
        // Generate and plot data
        std::vector<double> x(100), y(100);
        for (size_t i = 0; i < 100; ++i) {
            x[i] = 2.0 * M_PI * i / 99.0;
            y[i] = std::sin(x[i]);
        }
        plot->plot(x, y, COLOR_RED);
        
        // Display
        fig.show();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
```

## Building

### Prerequisites

- OpenGL 3.3+
- GLEW
- GLFW3
- CMake or Make

### Using Make

```bash
# Build library and examples
make all

# Build only the library
make library

# Build examples
make examples

# Run examples
./examples/simple_example_c
./examples/simple_example_cpp
```

### Using CMake

```bash
mkdir build && cd build
cmake ..
make
```

## API Reference

### Core Functions

- `cpl_create_figure(width, height)` - Create a new figure
- `cpl_add_plot(figure)` - Add a plot to the figure
- `cpl_show_figure(figure)` - Display the figure
- `cpl_free_figure(figure)` - Free figure resources

### Plot Configuration

- `cpl_set_x_range(plot, min, max)` - Set X-axis range
- `cpl_set_y_range(plot, min, max)` - Set Y-axis range
- `cpl_set_title(plot, title)` - Set plot title
- `cpl_show_grid(plot, show)` - Toggle grid display

### Data Plotting

- `cpl_plot(plot, x, y, n_points, color, color_fn, user_data)` - Plot data
- `cpl_plot_parametric(plot, t, x, y, n_points, color, color_fn, user_data)` - Plot parametric curve

## What's New in v2.0

### Improvements

1. **Unified API**: Consistent `cpl_` prefix for all functions
2. **Better Error Handling**: Proper error checking and reporting
3. **Memory Management**: Improved resource management and cleanup
4. **Code Organization**: Cleaner separation of concerns
5. **Performance**: Optimized rendering pipeline
6. **Documentation**: Comprehensive API documentation

### Breaking Changes

- Function names now use `cpl_` prefix
- Some function signatures have changed for consistency
- C++ interface uses modern C++ features

## Migration from v1.x

### C API Changes

```c
// Old v1.x
CPLFigure* fig = CreateFigure(800, 600);
CPLPlot* plot = AddPlot(fig);
SetXRange(plot, x_range);
Plot(plot, x, y, n, COLOR_RED, NULL, NULL);
ShowFigure(fig);

// New v2.0
CPLFigure* fig = cpl_create_figure(800, 600);
CPLPlot* plot = cpl_add_plot(fig);
cpl_set_x_range(plot, 0.0, 2.0 * M_PI);
cpl_plot(plot, x, y, n, COLOR_RED, NULL, NULL);
cpl_show_figure(fig);
```

### C++ API Changes

```cpp
// Old v1.x
cpl::Figure fig(800, 600);
auto plot = fig.addPlot();
plot->setXRange(0.0, 2.0 * M_PI);
plot->plot(x, y, COLOR_RED);

// New v2.0 (mostly the same, but with better error handling)
cpl::Figure fig(800, 600);
auto plot = fig.addPlot();
plot->setXRange(0.0, 2.0 * M_PI);
plot->plot(x, y, COLOR_RED);
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## License

MIT License - see LICENSE file for details.

## Support

- GitHub Issues: Report bugs and request features
- Documentation: Check the examples and API reference
- Community: Join discussions in GitHub Discussions
