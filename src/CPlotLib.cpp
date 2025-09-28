#include "CPlotLib.hpp"
#include <stdexcept>

namespace cpl {

// Figure implementation
Figure::Figure(size_t width, size_t height) 
    : fig_(cpl_create_figure(width, height)) {
    if (!fig_) {
        throw std::runtime_error("Failed to create figure");
    }
}

Figure::~Figure() {
    if (fig_) {
        cpl_free_figure(fig_);
    }
}

Figure::Figure(Figure&& other) noexcept 
    : fig_(other.fig_), plots_(std::move(other.plots_)) {
    other.fig_ = nullptr;
}

Figure& Figure::operator=(Figure&& other) noexcept {
    if (this != &other) {
        if (fig_) {
            cpl_free_figure(fig_);
        }
        fig_ = other.fig_;
        plots_ = std::move(other.plots_);
        other.fig_ = nullptr;
    }
    return *this;
}

Plot* Figure::addPlot() {
    if (!fig_) {
        throw std::runtime_error("Cannot add plot to freed figure");
    }
    
    ::CPLPlot* c_plot = cpl_add_plot(fig_);
    if (!c_plot) {
        throw std::runtime_error("Failed to add plot");
    }
    
    auto plot = std::make_unique<Plot>(c_plot);
    Plot* plot_ptr = plot.get();
    plots_.push_back(std::move(plot));
    
    return plot_ptr;
}

std::vector<Plot*> Figure::addSubplots(size_t rows, size_t cols) {
    if (!fig_) {
        throw std::runtime_error("Cannot add subplots to freed figure");
    }
    
    cpl_add_subplots(fig_, rows, cols);
    
    std::vector<Plot*> subplot_ptrs;
    for (size_t i = plots_.size(); i < fig_->num_plots; ++i) {
        auto plot = std::make_unique<Plot>(fig_->plots[i]);
        Plot* plot_ptr = plot.get();
        plots_.push_back(std::move(plot));
        subplot_ptrs.push_back(plot_ptr);
    }
    
    return subplot_ptrs;
}

Plot* Figure::getSubplot(size_t index) {
    if (!fig_) {
        throw std::runtime_error("Cannot get subplot from freed figure");
    }
    
    ::CPLPlot* c_plot = cpl_get_subplot(fig_, index);
    if (!c_plot) {
        return nullptr;
    }
    
    // Find the corresponding C++ Plot object
    for (auto& plot : plots_) {
        if (plot->getCPlot() == c_plot) {
            return plot.get();
        }
    }
    
    return nullptr;
}

Plot* Figure::getSubplotAt(size_t row, size_t col, size_t rows, size_t cols) {
    if (!fig_) {
        throw std::runtime_error("Cannot get subplot from freed figure");
    }
    
    ::CPLPlot* c_plot = cpl_get_subplot_at(fig_, row, col, rows, cols);
    if (!c_plot) {
        return nullptr;
    }
    
    // Find the corresponding C++ Plot object
    for (auto& plot : plots_) {
        if (plot->getCPlot() == c_plot) {
            return plot.get();
        }
    }
    
    return nullptr;
}

void Figure::show() {
    if (fig_) {
        cpl_show_figure(fig_);
        fig_ = nullptr; // Mark as freed
    }
}

void Figure::save(const std::string& filename) {
    if (fig_) {
        cpl_save_figure(fig_, filename.c_str());
    }
}

// Plot implementation
Plot::Plot(::CPLPlot* plot) : plot_(plot) {
    if (!plot_) {
        throw std::runtime_error("Invalid plot pointer");
    }
}

Plot::Plot(Plot&& other) noexcept : plot_(other.plot_) {
    other.plot_ = nullptr;
}

Plot& Plot::operator=(Plot&& other) noexcept {
    if (this != &other) {
        plot_ = other.plot_;
        other.plot_ = nullptr;
    }
    return *this;
}

void Plot::setXRange(double min, double max) {
    if (min >= max) {
        throw std::invalid_argument("X range: min must be less than max");
    }
    cpl_set_x_range(plot_, min, max);
}

void Plot::setYRange(double min, double max) {
    if (min >= max) {
        throw std::invalid_argument("Y range: min must be less than max");
    }
    cpl_set_y_range(plot_, min, max);
}

void Plot::setTitle(const std::string& title) {
    cpl_set_title(plot_, title.c_str());
}

void Plot::setXLabel(const std::string& label) {
    cpl_set_x_label(plot_, label.c_str());
}

void Plot::setYLabel(const std::string& label) {
    cpl_set_y_label(plot_, label.c_str());
}

void Plot::showGrid(bool show) {
    cpl_show_grid(plot_, show);
}

void Plot::setBackgroundColor(const Color& color) {
    cpl_set_background_color(plot_, color);
}

// Line thickness control
void Plot::setLineWidth(float width) {
    if (width <= 0.0f) {
        throw std::invalid_argument("Line width must be positive");
    }
    cpl_set_line_width(plot_, width);
}

void Plot::setGridLineWidth(float width) {
    if (width <= 0.0f) {
        throw std::invalid_argument("Grid line width must be positive");
    }
    cpl_set_grid_line_width(plot_, width);
}

void Plot::setBoxLineWidth(float width) {
    if (width <= 0.0f) {
        throw std::invalid_argument("Box line width must be positive");
    }
    cpl_set_box_line_width(plot_, width);
}

void Plot::plot(const std::vector<double>& x, const std::vector<double>& y, 
                const Color& color, CPLColorCallback color_fn, void* user_data) {
    if (x.size() != y.size()) {
        throw std::invalid_argument("x and y vectors must have the same size");
    }
    
    cpl_plot(plot_, x.data(), y.data(), x.size(), color, color_fn, user_data);
}

void Plot::plotParametric(const std::vector<double>& t, 
                         const std::vector<double>& x, 
                         const std::vector<double>& y, 
                         const Color& color, CPLColorCallback color_fn, 
                         void* user_data) {
    if (t.size() != x.size() || t.size() != y.size()) {
        throw std::invalid_argument("t, x, and y vectors must have the same size");
    }
    
    cpl_plot_parametric(plot_, t.data(), x.data(), y.data(), t.size(), 
                       color, color_fn, user_data);
}

void Plot::setSubplotLayout(size_t rows, size_t cols, size_t index) {
    if (!plot_) {
        throw std::runtime_error("Cannot set subplot layout on freed plot");
    }
    
    cpl_set_subplot_layout(plot_, rows, cols, index);
}

void Plot::setSubplotMargin(float margin) {
    if (!plot_) {
        throw std::runtime_error("Cannot set subplot margin on freed plot");
    }
    
    cpl_set_subplot_margin(plot_, margin);
}

} // namespace cpl
