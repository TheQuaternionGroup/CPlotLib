#include "CPlotLib.h"
#include <stdio.h>

double fx(double t)
{
    return 0.5 * t;
}

double fy(double t)
{
    return 0.5 * t * t;
}

int main()
{
    CPLFigure* fig = CreateFigure(800, 600);
    
    if (!fig)
    {
        printf("Error: Could not create figure.\n");
        return 1;
    }

    CPLPlot plot;
    AddSubplots(fig, 2, 2);

    // printf("Plotting parametric function: x(t) = 0.5 * t, y(t) = 0.5 * t^2\n");

    ShowFigure(fig);

    return 0;
}