/*
    * CPLPlot.h
    * This file contains the definition of the CPLPlot struct and its associated functions.
    * The CPLPlot struct is used to represent a plot in the CPlotLib library.
 */

#ifndef CPL_PLOT_HPP
#define CPL_PLOT_HPP

#include "CPLCore.h"
#include "CPLColors.h"

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CPL_PLOT_GL CPL_PLOT_GL;

struct CPLPlot
{
    CPLFigure* figure;          // The figure to which this plot belongs

    size_t width;               // The width of the plot
    size_t height;              // The height of the plot

    const char title[64];       // The title of the plot
    const char x_label[64];     // The x-axis label
    const char y_label[64];     // The y-axis label
    const char z_label[64];     // The z-axis label

    double x_range[2];          // The range of the x-axis
    double y_range[2];          // The range of the y-axis
    double z_range[2];          // The range of the z-axis

    double* x_data;             // The x-axis data
    double* y_data;             // The y-axis data
    double* z_data;             // The z-axis data

    size_t num_xticks;          // The number of x-axis ticks
    size_t num_yticks;          // The number of y-axis ticks
    size_t num_zticks;          // The number of z-axis ticks

    bool show_grid;             // Whether or not to show the grid
    bool show_axes;             // Whether or not to show the axes
    bool show_ticks;            // Whether or not to show the ticks

    Color bg_color;     // The background color of the plot

    CPL_PLOT_GL* gl_data;       // The OpenGL data for the plot

};

/*!
 * @brief Sets the x-axis data for the plot.
 * @param fig The plot to set the x-axis data for.
 * @param x_range The x-axis data.
*/
CPLAPI void SetXRange(CPLPlot* plot, double x_range[2]);

/*!
 * @brief Sets the y-axis data for the plot.
 * @param fig The plot to set the y-axis data for.
 * @param y_range The y-axis data.
*/
CPLAPI void SetYRange(CPLPlot* plot, double y_range[2]);

/*!
 * @brief Sets the z-axis data for the plot.
 * @param fig The plot to set the z-axis data for.
 * @param z_range The z-axis data.
*/
CPLAPI void SetZRange(CPLPlot* plot, double z_range[2]);

/*!
 * @brief Sets the x-axis data for the plot.
 * @param fig The plot to set the x-axis data for.
 * @param x_data The x-axis data.
 * @param num_xticks The number of x-axis ticks.
 */
CPLAPI void SetXData(CPLPlot* plot, double* x_data, size_t num_xticks);

/*!
 * @brief Sets the y-axis data for the plot.
 * @param fig The plot to set the y-axis data for.
 * @param y_data The y-axis data.
 * @param num_yticks The number of y-axis ticks.
 */
CPLAPI void SetYData(CPLPlot* plot, double* y_data, size_t num_yticks);

/*!
 * @brief Sets the z-axis data for the plot.
 * @param fig The plot to set the z-axis data for.
 * @param z_data The z-axis data.
 * @param num_zticks The number of z-axis ticks.
 */
CPLAPI void SetZData(CPLPlot* plot, double* z_data, size_t num_zticks);

/*!
 * @brief Sets the background color of the plot.
 * @param fig The plot to set the background color for.
 * @param bg_color The background color.
 */
void DrawPlot(CPLPlot* plot);

/*!
* @brief Frees the memory associated with the plot.
* @param plot The plot to free.
*/
void FreePlot(CPLPlot* plot);

#ifdef __cplusplus
}
#endif

#endif // CPL_PLOT_HPP