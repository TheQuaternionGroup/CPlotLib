#include "CPLPlot.h"
#include "utils/CPLRenderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>

// Internal function declarations
static void cpl_render_plot_internal(CPLPlot* plot);
static void cpl_plot_error(const char* message);

// Rendering functions
void cpl_render_plot(CPLPlot* plot) {
    if (!plot || !plot->data) {
        cpl_plot_error("Invalid plot or data");
        return;
    }
    
    cpl_render_plot_internal(plot);
}

// Internal helper functions
static void cpl_render_plot_internal(CPLPlot* plot) {
    if (!plot || !plot->data) return;
    
    // Note: Shader program and projection matrix are set once per frame in the render loop
    // This eliminates redundant OpenGL state changes
    
    // Draw plot box
    if (plot->data->box_loaded) {
        glLineWidth(plot->box_line_width);
        glBindVertexArray(plot->data->box_vao);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
        glBindVertexArray(0);
    }
    
    // Draw grid if enabled
    if (plot->show_grid && plot->data->grid_loaded) {
        // Set grid line width uniform for shader-based thickness control
        GLint line_width_location = glGetUniformLocation(plot->figure->renderer->program_id, "lineWidth");
        if (line_width_location != -1) {
            glUniform1f(line_width_location, plot->grid_line_width);
        }
        
        // Use shader-based line thickness for grid lines
        // This allows for sub-pixel line thickness control
        glBindVertexArray(plot->data->grid_vao);
        glDrawArrays(GL_LINES, 0, 44); // 22 lines * 2 vertices each (11 horizontal + 11 vertical)
        glBindVertexArray(0);
    }
    
    // Draw all lines
    for (size_t i = 0; i < plot->data->num_lines; i++) {
        CPLLine* line = &plot->data->lines[i];
        if (line->is_loaded) {
            glLineWidth(plot->line_width);
            glBindVertexArray(line->vao);
            glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)line->num_vertices);
            glBindVertexArray(0);
        }
    }
}

static void cpl_plot_error(const char* message) {
    fprintf(stderr, "CPlotLib Error: %s\n", message);
}
