// CPLFigure.hpp

#ifndef CPL_FIGURE_HPP
#define CPL_FIGURE_HPP

#include "../C_headers/CPLFigure.h"
#include "../C_headers/CPLPlot.h"

#include "CPLPlot.hpp" // Include Plot wrapper

#include <string>
#include <vector>
#include <stdexcept>
#include <memory>

namespace cpl {

/**
 * @brief C++ wrapper for CPLFigure.
 *
 * This class manages a CPLFigure instance. It allows adding plots and subplots,
 * configuring them, and displaying the figure. The actual resource freeing is
 * handled by the ShowFigure function, so destructors and move operations do
 * not attempt to free resources to prevent double freeing.
 */
class Figure {
public:
    /**
     * @brief Constructs a Figure with specified width and height.
     * @param width The width of the figure in pixels.
     * @param height The height of the figure in pixels.
     * @throws std::runtime_error if figure creation fails.
     */
    explicit Figure(size_t width, size_t height) {
        fig_ = CreateFigure(width, height);
        if (!fig_) {
            throw std::runtime_error("Failed to create CPLFigure.");
        }
    }

    /**
     * @brief Destructor does not free CPLFigure as ShowFigure handles it.
     */
    ~Figure() = default;

    // Disable copy semantics
    Figure(const Figure&) = delete;
    Figure& operator=(const Figure&) = delete;

    // Enable move semantics
    Figure(Figure&& other) noexcept 
        : fig_(other.fig_), plots_(std::move(other.plots_)) {
        other.fig_ = nullptr;
    }

    Figure& operator=(Figure&& other) noexcept {
        if (this != &other) {
            // Do not free fig_ to prevent double free
            fig_ = other.fig_;
            plots_ = std::move(other.plots_);
            other.fig_ = nullptr;
        }
        return *this;
    }

    /**
     * @brief Displays the figure window and frees resources.
     *
     * After calling this method, the Figure instance should not be used further,
     * as the underlying resources have been freed.
     */
    void show() {
        if (fig_) {
            ShowFigure(fig_);
            fig_ = nullptr; // Mark as freed to prevent further usage
        }
    }

    /**
     * @brief Saves the figure to a file.
     * @param filename The name of the file to save the figure to.
     *
     * @note This does not free any resources.
     */
    void save(const std::string& filename) const {
        if (fig_) {
            SaveFigure(fig_, filename.c_str());
        }
    }

    /**
     * @brief Draws the figure on the screen.
     *
     * @note This does not free any resources.
     */
    void draw() const {
        if (fig_) {
            DrawFigure(fig_);
        }
    }

    /**
     * @brief Adds a new plot to the figure.
     * @return A raw pointer to the newly added Plot.
     * @throws std::runtime_error if plot creation fails.
     *
     * @note The Figure instance retains ownership of the Plot.
     */
    Plot* addPlot() {
        if (!fig_) {
            throw std::runtime_error("Cannot add plot to a freed Figure.");
        }
        CPLPlot* plot = AddPlot(fig_);
        if (!plot) {
            throw std::runtime_error("Failed to add CPLPlot to CPLFigure.");
        }
        auto cppPlot = std::make_unique<Plot>(plot);
        Plot* plotPtr = cppPlot.get();
        plots_.emplace_back(std::move(cppPlot));
        return plotPtr; // Return Plot* instead of unique_ptr<Plot>
    }

    /**
     * @brief Adds subplots arranged in a grid to the figure.
     * @param rows Number of subplot rows.
     * @param cols Number of subplot columns.
     * @return A vector of raw pointers to the newly added Plots.
     * @throws std::runtime_error if subplot creation fails.
     *
     * @note The Figure instance retains ownership of the Plots.
     */
    std::vector<Plot*> addSubplots(size_t rows, size_t cols) {
        if (!fig_) {
            throw std::runtime_error("Cannot add subplots to a freed Figure.");
        }
        AddSubplots(fig_, rows, cols);
        std::vector<Plot*> subplotWrappers;

        for (size_t i = 0; i < fig_->num_plots; ++i) {
            CPLPlot* plot = fig_->plot[i];
            if (plot) {
                auto cppPlot = std::make_unique<Plot>(plot);
                Plot* plotPtr = cppPlot.get();
                plots_.emplace_back(std::move(cppPlot));
                subplotWrappers.emplace_back(plotPtr);
            }
        }

        return subplotWrappers; // Return vector of Plot* instead of unique_ptr<Plot>
    }

    /**
     * @brief Gets the underlying CPLFigure pointer.
     * @return Pointer to CPLFigure.
     */
    ::CPLFigure* getCPLFigure() const {
        return fig_;
    }

    /**
     * @brief Retrieves a Plot by index.
     * @param index The subplot index (0-based).
     * @return Pointer to the requested Plot.
     * @throws std::out_of_range if the index is invalid.
     */
    Plot* getPlot(size_t index) const {
        if (index >= plots_.size()) {
            throw std::out_of_range("Plot index out of range.");
        }
        return plots_[index].get();
    }

private:
    ::CPLFigure* fig_; // Raw pointer to CPLFigure; ownership not managed here
    mutable std::vector<std::unique_ptr<Plot>> plots_; // Stores all Plot wrappers
};

} // namespace cpl

#endif // CPL_FIGURE_HPP
