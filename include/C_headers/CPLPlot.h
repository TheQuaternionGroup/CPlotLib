/*
    * CPLPlot.h
    * This file contains the definition of the CPLPlot struct and its associated functions.
    * The CPLPlot struct is used to represent a plot in the CPlotLib library.
 */

#ifndef CPL_PLOT_H
#define CPL_PLOT_H

#include "CPLCore.h"
#include "CPLColors.h"

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration of CPLFigure
typedef struct CPLFigure CPLFigure;

/*!
 * @brief Structure representing a single line within a plot.
 */
typedef struct CPLLine CPLLine;

/*!
 * @brief Structure representing a plot within a figure.
 */
typedef struct CPL_PLOT_GL CPL_PLOT_GL;

typedef struct CPLPlot
{
    CPLFigure* figure;          // The figure to which this plot belongs

    size_t width;               // The width of the plot
    size_t height;              // The height of the plot

    char title[64];             // The title of the plot
    char x_label[64];           // The x-axis label
    char y_label[64];           // The y-axis label
    char z_label[64];           // The z-axis label

    double x_range[2];          // The range of the x-axis
    double y_range[2];          // The range of the y-axis
    double z_range[2];          // The range of the z-axis

    bool show_grid;             // Whether or not to show the grid
    bool show_axes;             // Whether or not to show the axes
    bool show_ticks;            // Whether or not to show the ticks

    Color bg_color;             // The background color of the plot

    CPL_PLOT_GL* gl_data;       // The OpenGL data for the plot

    CPLLine* lines;             // Dynamic array of lines
    size_t num_lines;           // Number of lines in the plot
} CPLPlot;

/*!
 * @brief Sets the x-axis range for the plot.
 * @param plot The plot to set the x-axis range for.
 * @param x_range The x-axis range as an array of two doubles.
 */
CPLAPI void SetXRange(CPLPlot* plot, double x_range[2]);

/*!
 * @brief Sets the y-axis range for the plot.
 * @param plot The plot to set the y-axis range for.
 * @param y_range The y-axis range as an array of two doubles.
 */
CPLAPI void SetYRange(CPLPlot* plot, double y_range[2]);

/*!
 * @brief Sets the z-axis range for the plot.
 * @param plot The plot to set the z-axis range for.
 * @param z_range The z-axis range as an array of two doubles.
 */
CPLAPI void SetZRange(CPLPlot* plot, double z_range[2]);

/*!
 * @brief Adds a plot to the figure.
 * @param fig The figure to add the plot to.
 * @return Pointer to the newly added plot.
 */
CPLAPI CPLPlot* AddPlot(CPLFigure* fig);

/*!
 * @brief Adds subplots to the figure.
 * @param fig The figure to add the subplots to.
 * @param rows The number of rows of subplots.
 * @param cols The number of columns of subplots.
 */
CPLAPI void AddSubplots(CPLFigure* fig, size_t rows, size_t cols);

/*!
 * @brief Function pointer type for color callbacks.
 * @param t The parameter (e.g., x-value) passed to the callback.
 * @param userData User-defined data passed to the callback.
 * @return The color corresponding to the parameter.
 */
typedef Color (*ColorCallback)(double t, void* userData);

/*!
 * @brief Adds a line to the plot.
 * @param plot The plot to add the line to.
 * @param x_arr Array of x-coordinates.
 * @param y_arr Array of y-coordinates.
 * @param num_points Number of points in the arrays.
 * @param line_color The color of the line.
 * @param color_fn Optional callback to determine color dynamically.
 * @param user_data Optional user data for the color callback.
 */
CPLAPI void Plot(
    CPLPlot* plot,
    double* x_arr,
    double* y_arr,
    size_t num_points,
    Color line_color,
    ColorCallback color_fn,
    void* user_data
);

CPLAPI void PlotParamCurve(
    CPLPlot* plot,
    double* t_arr,
    double* x_arr,
    double* y_arr,
    size_t num_points,
    Color line_color,
    ColorCallback color_fn,
    void* user_data
);

/*!
 * @brief Renders the plot.
 * @param plot The plot to render.
 */
CPLAPI void DrawPlot(CPLPlot* plot);

/*!
 * @brief Frees the memory associated with the plot.
 * @param plot The plot to free.
 */
CPLAPI void FreePlot(CPLPlot* plot);

#ifdef __cplusplus
}
#endif

#endif // CPL_PLOT_H
