#ifndef CPL_FIGURE_H
#define CPL_FIGURE_H

#include "CPLCore.h"
#include "CPLColors.h"

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CPLFigure
{
    CPLRenderer* renderer;      // The renderer for the figure
    CPLPlot** plot;              // The plot to which this figure belongs
    size_t num_plots;           // The number of plots in the figure

    size_t width;               // The width of the figure
    size_t height;              // The height of the figure

    Color bg_color;                // The color of the figure
};

/*!
 * @brief Creates a new figure.
 * @param width The width of the figure.
 * @param height The height of the figure.
 * @param title The title of the figure.
 * @return A pointer to the newly created figure.
 */
CPLAPI CPLFigure* CreateFigure(size_t width, size_t height);

/*!
 * @brief Adds a plot to the figure.
 * @param fig The figure to add the plot to.
 * @param plot The plot to add.
 */
CPLAPI void AddPlot(CPLFigure* fig, CPLPlot* plot);

/*!
 * @brief Adds subplots to the figure.
 * @param fig The figure to add the subplots to.
 * @param rows The number of rows of subplots.
 * @param cols The number of columns of subplots.
 */
CPLAPI void AddSubplots(CPLFigure* fig, size_t rows, size_t cols);

/*!
* @brief Runs the run loop for the plot window to display the plot.
* @param plot The plot to display.
*/
CPLAPI void ShowFigure(CPLFigure* fig);

/*!
* @brief Saves the plot to a file.
* @param plot The plot to save.
* @param filename The name of the file to save the plot to.
*/
CPLAPI void SaveFigure(CPLFigure* fig, const char* filename);

/*!
 * @brief Draws the figure to the screen.
 * @param fig The figure to draw.
 */
void DrawFigure(CPLFigure* fig);

/*!
 * @brief Frees the memory associated with the figure.
 * @param fig The figure to free.
 */
void FreeFigure(CPLFigure* fig);


#ifdef __cplusplus
}
#endif

#endif // CPL_FIGURE_H