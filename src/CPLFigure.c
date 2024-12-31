#include "CPLFigure.h"
#include "CPLPlot.h"
#include "utils/cpl_renderer.h"

#include <stdio.h>
#include <stdlib.h>

CPLFigure* CreateFigure(size_t width, size_t height)
{
    // Allocate memory for the figure
    CPLFigure* fig = (CPLFigure*)malloc(sizeof(CPLFigure));
    if (!fig) {
        printf("Error: Could not allocate memory for figure.\n");
        return NULL;
    }

    // Initialize the renderer
    fig->renderer = cpl_init_renderer(width, height);
    if (!fig->renderer) {
        printf("Error: Could not initialize renderer.\n");
        free(fig);
        return NULL;
    }

    fig->plot = NULL;
    fig->num_plots = 0;
    fig->width = width;
    fig->height = height;
    fig->bg_color = COLOR_WHITE; // figure background is white

    return fig;
}

void ShowFigure(CPLFigure* fig)
{
    if (!fig) {
        printf("Error: Figure is NULL.\n");
        return;
    }

    while (!glfwWindowShouldClose(fig->renderer->window)) {
        // Clear the screen with the figure's background
        glClearColor(
            fig->bg_color.r,
            fig->bg_color.g,
            fig->bg_color.b,
            fig->bg_color.a
        );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get current framebuffer size (in case of hi-DPI, etc.)
        int fb_width, fb_height;
        glfwGetFramebufferSize(fig->renderer->window, &fb_width, &fb_height);
        glViewport(0, 0, fb_width, fb_height);

        // Draw all plots
        for (size_t i = 0; i < fig->num_plots; i++) {
            if (fig->plot[i]) {
                DrawPlot(fig->plot[i]);
            }
        }

        // Poll for events and check ESC
        glfwPollEvents();
        if (GLFW_PRESS == glfwGetKey(fig->renderer->window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(fig->renderer->window, 1);
        }

        // Swap buffers
        glfwSwapBuffers(fig->renderer->window);
    }

    // Terminate and free
    glfwTerminate();
    FreeFigure(fig);
}

void FreeFigure(CPLFigure* fig)
{
    if (!fig) {
        printf("Error: Could not free figure. NULL pointer.\n");
        return;
    }

    // Free each plot
    if (fig->plot) {
        for (size_t i = 0; i < fig->num_plots; i++) {
            if (fig->plot[i]) {
                // First free GL resources inside the plot
                FreePlot(fig->plot[i]);
                // Then free the plot struct itself
                free(fig->plot[i]);
            }
        }
        free(fig->plot);
    }

    // Free the renderer
    cpl_free_renderer(fig->renderer);

    // Finally free the figure struct
    free(fig);
}
