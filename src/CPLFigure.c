#include "CPLFigure.h"
#include "CPLPlot.h"
#include "utils/cpl_renderer.h"

#include <stdio.h>

CPLFigure* CreateFigure(size_t width, size_t height)
{
    // Allocate memory for the figure
    CPLFigure* fig = (CPLFigure*)malloc(sizeof(CPLFigure));
    if (!fig)
    {
        printf("Error: Could not allocate memory for figure.\n");
        return NULL;
    }
    // Initialize the renderer
    fig->renderer = cpl_init_renderer(width, height);
    if (!fig->renderer)
    {
        printf("Error: Could not initialize renderer.\n");
        free(fig);
        return NULL;
    }
    // Set the width and height of the figure
    fig->width = width;
    fig->height = height;

    // Set the color of the figure
    fig->bg_color = COLOR_WHITE;

    return fig;
}

void ShowFigure(CPLFigure* fig)
{
    if (!fig)
    {
        printf("Error: Figure is NULL.\n");
        return;
    }

    // Main loop
    while (!glfwWindowShouldClose(fig->renderer->window))
    {
        glClearColor(fig->bg_color.r, fig->bg_color.g, fig->bg_color.b, fig->bg_color.a);
        // Clear the screen
        int fb_width, fb_height;
        glfwGetFramebufferSize(fig->renderer->window, &fb_width, &fb_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, fb_width, fb_height);

        DrawPlot(fig->plot[0]);
        // Poll for events
        glfwPollEvents();

        if ( GLFW_PRESS == glfwGetKey( fig->renderer->window, GLFW_KEY_ESCAPE ) ) 
        { 
            glfwSetWindowShouldClose( fig->renderer->window, 1 ); 
        }

        // Draw the plots
        // if (!fig->plot)
        // {
        //     for (size_t i = 0; i < fig->num_plots; i++)
        //     {
        //         DrawPlot(fig->plot[i]);
        //     }
        // }
        // Swap buffers
        glfwSwapBuffers(fig->renderer->window);

    }

    // Terminate GLFW
    glfwTerminate();

    // Free the figure
    FreeFigure(fig);
}

void FreeFigure(CPLFigure* fig)
{
    if (!fig)
    {
        printf("Error: Could not free figure.\n");
        return;
    }   

    // free the renderer
    cpl_free_renderer(fig->renderer);

    // free the plots
    // for (size_t i = 0; i < fig->num_plots; i++)
    // {
    //     if (fig->plot[i]->gl_data) {
    //         if (fig->plot[i]->gl_data->box_vbo) glDeleteBuffers(1, &fig->plot[i]->gl_data->box_vbo);
    //         if (fig->plot[i]->gl_data->box_vao) glDeleteVertexArrays(1, &fig->plot[i]->gl_data->box_vao);
    //         free(fig->plot[i]->gl_data);
    //     }
    //     free(fig->plot[i]->x_data);
    //     free(fig->plot[i]->y_data);
    //     free(fig->plot[i]->z_data);
    // }
    free(fig->plot);

    free(fig);
}