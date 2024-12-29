#ifndef CPL_PLOT_HPP
#define CPL_PLOT_HPP

#include "CPLFigure.h"
#include "CPLColors.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A user-defined callback that, given a parameter t,
 * returns a color (e.g. for curvature or any other property).
 * If NULL, we use a uniform color.
 */
typedef Color (*ColorCallback)(double t, void* userData);

/**
 * Plots a 2D parametric function:
 *   x(t), y(t)
 * for t in [tMin, tMax], sampling 'numSamples' times.
 *
 * If colorFn is non-NULL, we call colorFn(t) for each sample
 * to color that segment of the curve.
 * If colorFn is NULL, we use 'lineColor' for the entire curve.
 */
void PlotParam2D(
    Figure* fig,
    double (*fx)(double),
    double (*fy)(double),
    double tMin,
    double tMax,
    int numSamples,
    Color lineColor,
    ColorCallback colorFn,
    void* userData
);

/**
 * Draws the figure (axes, ticks, curves) to the screen.
 */
void PlotDraw(Figure *fig);

/**
 * Enters the main loop, shows the figure, and then frees everything upon exit.
 */
void ShowPlot(Figure *fig);

#ifdef __cplusplus
}
#endif

#endif // CPL_PLOT_HPP
