// CPLPlot.hpp

#ifndef CPL_PLOT_HPP
#define CPL_PLOT_HPP

#include "../C_headers/CPLPlot.h"
#include "../C_headers/CPLColors.h"
#include "../C_headers/CPLFigure.h"

#include <vector>
#include <stdexcept>
#include <string>
#include <cstring> // For strncpy

namespace cpl {

/**
 * @brief C++ wrapper for CPLPlot.
 *
 * This class provides a convenient C++ interface for manipulating CPLPlot instances.
 * It does not manage the lifetime of the underlying CPLPlot* to prevent double freeing.
 */
class Plot {
public:
    /**
     * @brief Constructs a Plot associated with a CPLPlot pointer.
     * @param plot Pointer to an existing CPLPlot.
     * @throws std::runtime_error if plot is null.
     */
    explicit Plot(::CPLPlot* plot) : plot_(plot) {
        if (!plot_) {
            throw std::runtime_error("Invalid CPLPlot pointer.");
        }
    }

    /**
     * @brief Destructor does not free CPLPlot as Figure::show() handles it.
     */
    ~Plot() = default;

    // Disable copy semantics
    Plot(const Plot&) = delete;
    Plot& operator=(const Plot&) = delete;

    // Enable move semantics
    Plot(Plot&& other) noexcept 
        : plot_(other.plot_) {
        other.plot_ = nullptr;
    }

    Plot& operator=(Plot&& other) noexcept {
        if (this != &other) {
            plot_ = other.plot_;
            other.plot_ = nullptr;
        }
        return *this;
    }

    /**
     * @brief Sets the x-axis range.
     * @param min Minimum x-value.
     * @param max Maximum x-value.
     */
    void setXRange(double min, double max) {
        double range[2] = { min, max };
        ::SetXRange(plot_, range);
    }

    /**
     * @brief Sets the y-axis range.
     * @param min Minimum y-value.
     * @param max Maximum y-value.
     */
    void setYRange(double min, double max) {
        double range[2] = { min, max };
        ::SetYRange(plot_, range);
    }

    /**
     * @brief Sets the z-axis range.
     * @param min Minimum z-value.
     * @param max Maximum z-value.
     */
    void setZRange(double min, double max) {
        double range[2] = { min, max };
        ::SetZRange(plot_, range);
    }

    /**
     * @brief Plots a line on the plot.
     * @param x Array of x-coordinates.
     * @param y Array of y-coordinates.
     * @param color The color of the line.
     * @param colorFn Optional callback for dynamic coloring.
     * @param userData Optional user data for the color callback.
     * @throws std::invalid_argument if x and y sizes do not match.
     */
    void plot(
        const std::vector<double>& x,
        const std::vector<double>& y,
        const Color& color,
        ColorCallback colorFn = nullptr,
        void* userData = nullptr
    ) {
        if (x.size() != y.size()) {
            throw std::invalid_argument("x and y vectors must have the same size.");
        }
        ::Plot(plot_, const_cast<double*>(x.data()), const_cast<double*>(y.data()), x.size(), color, colorFn, userData);
    }

    /**
     * @brief Plots a parametric curve on the plot.
     * @param t Array of parameter values.
     * @param x Array of x-coordinates.
     * @param y Array of y-coordinates.
     * @param color The color of the curve.
     * @param colorFn Optional callback for dynamic coloring.
     * @param userData Optional user data for the color callback.
     * @throws std::invalid_argument if t, x, and y sizes do not match.
     */
    void plotParamCurve(
        const std::vector<double>& t,
        const std::vector<double>& x,
        const std::vector<double>& y,
        const Color& color,
        ColorCallback colorFn = nullptr,
        void* userData = nullptr
    ) {
        if (t.size() != x.size() || t.size() != y.size()) {
            throw std::invalid_argument("t, x, and y vectors must have the same size.");
        }
        ::PlotParamCurve(plot_, const_cast<double*>(t.data()), const_cast<double*>(x.data()), const_cast<double*>(y.data()), t.size(), color, colorFn, userData);
    }

    /**
     * @brief Sets the title of the plot.
     * @param title The title string.
     */
    void setTitle(const std::string& title) {
        strncpy(plot_->title, title.c_str(), sizeof(plot_->title) - 1);
        plot_->title[sizeof(plot_->title) - 1] = '\0';
    }

    /**
     * @brief Sets the x-axis label.
     * @param label The x-axis label string.
     */
    void setXLabel(const std::string& label) {
        strncpy(plot_->x_label, label.c_str(), sizeof(plot_->x_label) - 1);
        plot_->x_label[sizeof(plot_->x_label) - 1] = '\0';
    }

    /**
     * @brief Sets the y-axis label.
     * @param label The y-axis label string.
     */
    void setYLabel(const std::string& label) {
        strncpy(plot_->y_label, label.c_str(), sizeof(plot_->y_label) - 1);
        plot_->y_label[sizeof(plot_->y_label) - 1] = '\0';
    }

    /**
     * @brief Sets the z-axis label.
     * @param label The z-axis label string.
     */
    void setZLabel(const std::string& label) {
        strncpy(plot_->z_label, label.c_str(), sizeof(plot_->z_label) - 1);
        plot_->z_label[sizeof(plot_->z_label) - 1] = '\0';
    }

    /**
     * @brief Enables or disables grid display.
     * @param enable True to show grid, false to hide.
     */
    void showGrid(bool enable) {
        plot_->show_grid = enable ? true : false;
    }

    /**
     * @brief Enables or disables axes display.
     * @param enable True to show axes, false to hide.
     */
    void showAxes(bool enable) {
        plot_->show_axes = enable ? true : false;
    }

    /**
     * @brief Enables or disables ticks display.
     * @param enable True to show ticks, false to hide.
     */
    void showTicks(bool enable) {
        plot_->show_ticks = enable ? true : false;
    }

    /**
     * @brief Sets the background color of the plot.
     * @param color The background color.
     */
    void setBackgroundColor(const Color& color) {
        plot_->bg_color = color;
    }

    /**
     * @brief Gets the underlying CPLPlot pointer.
     * @return Pointer to CPLPlot.
     */
    ::CPLPlot* getCPLPlot() const {
        return plot_;
    }

private:
    ::CPLPlot* plot_; // Raw pointer to CPLPlot; ownership not managed here
};

} // namespace cpl

#endif // CPL_PLOT_HPP
