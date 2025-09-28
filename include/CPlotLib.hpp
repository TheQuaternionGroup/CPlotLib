/**
 * @file CPlotLib.hpp
 * @brief C++ wrapper for CPlotLib
 * @version 2.0.0
 * @author CPlotLib Team
 * 
 * This header provides C++ wrappers for the CPlotLib C API, offering
 * RAII semantics, STL containers, and modern C++ features while maintaining
 * full compatibility with the underlying C implementation.
 */

#ifndef CPLOTLIB_HPP
#define CPLOTLIB_HPP

#include "CPlotLib.h"
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>

namespace cpl {

// C++ wrapper for CPLFigure
class Figure {
public:
    explicit Figure(size_t width, size_t height);
    ~Figure();
    
    // Disable copy constructor and assignment
    Figure(const Figure&) = delete;
    Figure& operator=(const Figure&) = delete;
    
    // Enable move constructor and assignment
    Figure(Figure&& other) noexcept;
    Figure& operator=(Figure&& other) noexcept;
    
    // Plot management
    class Plot* addPlot();
    std::vector<class Plot*> addSubplots(size_t rows, size_t cols);
    class Plot* getSubplot(size_t index);
    class Plot* getSubplotAt(size_t row, size_t col, size_t rows, size_t cols);
    
    // Display and save
    void show();
    void save(const std::string& filename);
    
    // Getters
    size_t getWidth() const { return fig_->width; }
    size_t getHeight() const { return fig_->height; }
    size_t getNumPlots() const { return fig_->num_plots; }
    
    // Get underlying C figure (for advanced usage)
    ::CPLFigure* getCFigure() const { return fig_; }

private:
    ::CPLFigure* fig_;
    std::vector<std::unique_ptr<class Plot>> plots_;
};

// C++ wrapper for CPLPlot
class Plot {
public:
    explicit Plot(::CPLPlot* plot);
    ~Plot() = default;
    
    // Disable copy constructor and assignment
    Plot(const Plot&) = delete;
    Plot& operator=(const Plot&) = delete;
    
    // Enable move constructor and assignment
    Plot(Plot&& other) noexcept;
    Plot& operator=(Plot&& other) noexcept;
    
    // Range setting
    void setXRange(double min, double max);
    void setYRange(double min, double max);
    
    // Labels and titles
    void setTitle(const std::string& title);
    void setXLabel(const std::string& label);
    void setYLabel(const std::string& label);
    
    // Display options
    void showGrid(bool show);
    void setBackgroundColor(const Color& color);
    
    // Line thickness control
    void setLineWidth(float width);
    void setGridLineWidth(float width);
    void setBoxLineWidth(float width);
    
    // Data plotting
    void plot(const std::vector<double>& x, const std::vector<double>& y, 
              const Color& color, CPLColorCallback color_fn = nullptr, 
              void* user_data = nullptr);
    void plotParametric(const std::vector<double>& t, 
                       const std::vector<double>& x, 
                       const std::vector<double>& y, 
                       const Color& color, CPLColorCallback color_fn = nullptr, 
                       void* user_data = nullptr);
    
    // Subplot management
    void setSubplotLayout(size_t rows, size_t cols, size_t index);
    void setSubplotMargin(float margin);
    
    // Getters
    ::CPLPlot* getCPlot() const { return plot_; }

private:
    ::CPLPlot* plot_;
};

} // namespace cpl

#endif // CPLOTLIB_HPP
