#include "CPLPlot.h"
#include "utils/CPLShader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>

// Internal function declarations
static void cpl_setup_plot_box(CPLPlot* plot);
static void cpl_setup_grid(CPLPlot* plot);
static void cpl_build_line_data(CPLPlot* plot, const double* x, const double* y, 
                               size_t n_points, Color color, 
                               CPLColorCallback color_fn, void* user_data);
static float cpl_normalize_coordinate(double value, double min, double max, 
                                     float plot_start, float plot_size);
static void cpl_plot_error(const char* message);

// Constants
#define CPL_DEFAULT_MARGIN 0.1f
#define CPL_INITIAL_CAPACITY 4

// Data plotting
void cpl_plot(CPLPlot* plot, const double* x, const double* y, size_t n_points, 
              Color color, CPLColorCallback color_fn, void* user_data) {
    if (!plot || !x || !y || n_points == 0) {
        cpl_plot_error("Invalid plot or data");
        return;
    }
    
    // Setup plot box and grid if not already done
    if (!plot->data->box_loaded) {
        cpl_setup_plot_box(plot);
    }
    if (plot->show_grid && !plot->data->grid_loaded) {
        cpl_setup_grid(plot);
    }
    
    // Build line data
    cpl_build_line_data(plot, x, y, n_points, color, color_fn, user_data);
}

void cpl_plot_parametric(CPLPlot* plot, const double* t, const double* x, 
                        const double* y, size_t n_points, Color color, 
                        CPLColorCallback color_fn, void* user_data) {
    if (!plot || !t || !x || !y || n_points == 0) {
        cpl_plot_error("Invalid plot or parametric data");
        return;
    }
    
    // For parametric plots, we use the t values as x and the x,y as coordinates
    // This is a simplified approach - in a full implementation, you might want
    // to handle the parametric nature differently
    cpl_plot(plot, x, y, n_points, color, color_fn, user_data);
}

// Internal helper functions
static void cpl_setup_plot_box(CPLPlot* plot) {
    if (!plot || !plot->data) return;
    
    // Generate box vertices (plot border)
    float vertices[20]; // 4 vertices * (2 coords + 3 color)
    float margin = plot->data->margin;
    
    // Bottom-left
    vertices[0] = -1.0f + margin;  // x
    vertices[1] = -1.0f + margin;  // y
    vertices[2] = 0.0f;            // r
    vertices[3] = 0.0f;            // g
    vertices[4] = 0.0f;            // b
    
    // Bottom-right
    vertices[5] = 1.0f - margin;   // x
    vertices[6] = -1.0f + margin;  // y
    vertices[7] = 0.0f;            // r
    vertices[8] = 0.0f;            // g
    vertices[9] = 0.0f;            // b
    
    // Top-right
    vertices[10] = 1.0f - margin;  // x
    vertices[11] = 1.0f - margin;  // y
    vertices[12] = 0.0f;           // r
    vertices[13] = 0.0f;           // g
    vertices[14] = 0.0f;           // b
    
    // Top-left
    vertices[15] = -1.0f + margin; // x
    vertices[16] = 1.0f - margin;  // y
    vertices[17] = 0.0f;           // r
    vertices[18] = 0.0f;           // g
    vertices[19] = 0.0f;           // b
    
    // Create OpenGL objects
    glGenVertexArrays(1, &plot->data->box_vao);
    glGenBuffers(1, &plot->data->box_vbo);
    
    glBindVertexArray(plot->data->box_vao);
    glBindBuffer(GL_ARRAY_BUFFER, plot->data->box_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    
    // Color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    plot->data->box_loaded = true;
}

static void cpl_setup_grid(CPLPlot* plot) {
    if (!plot || !plot->data) return;
    
    // Generate grid lines
    const int grid_lines = 10;
    const int total_vertices = (grid_lines * 2 + 2) * 2; // horizontal + vertical lines
    float* vertices = malloc(total_vertices * 5 * sizeof(float)); // 5 floats per vertex
    
    if (!vertices) {
        cpl_plot_error("Failed to allocate memory for grid");
        return;
    }
    
    int vertex_index = 0;
    float margin = plot->data->margin;
    
    // Horizontal grid lines
    for (int i = 0; i <= grid_lines; i++) {
        float y = -1.0f + margin + (2.0f - 2.0f * margin) * i / grid_lines;
        
        // Start vertex
        vertices[vertex_index * 5 + 0] = -1.0f + margin;  // x
        vertices[vertex_index * 5 + 1] = y;               // y
        vertices[vertex_index * 5 + 2] = 0.7f;            // r
        vertices[vertex_index * 5 + 3] = 0.7f;            // g
        vertices[vertex_index * 5 + 4] = 0.7f;            // b
        vertex_index++;
        
        // End vertex
        vertices[vertex_index * 5 + 0] = 1.0f - margin;   // x
        vertices[vertex_index * 5 + 1] = y;               // y
        vertices[vertex_index * 5 + 2] = 0.7f;            // r
        vertices[vertex_index * 5 + 3] = 0.7f;            // g
        vertices[vertex_index * 5 + 4] = 0.7f;            // b
        vertex_index++;
    }
    
    // Vertical grid lines
    for (int i = 0; i <= grid_lines; i++) {
        float x = -1.0f + margin + (2.0f - 2.0f * margin) * i / grid_lines;
        
        // Start vertex
        vertices[vertex_index * 5 + 0] = x;               // x
        vertices[vertex_index * 5 + 1] = -1.0f + margin;  // y
        vertices[vertex_index * 5 + 2] = 0.7f;            // r
        vertices[vertex_index * 5 + 3] = 0.7f;            // g
        vertices[vertex_index * 5 + 4] = 0.7f;            // b
        vertex_index++;
        
        // End vertex
        vertices[vertex_index * 5 + 0] = x;               // x
        vertices[vertex_index * 5 + 1] = 1.0f - margin;   // y
        vertices[vertex_index * 5 + 2] = 0.7f;            // r
        vertices[vertex_index * 5 + 3] = 0.7f;            // g
        vertices[vertex_index * 5 + 4] = 0.7f;            // b
        vertex_index++;
    }
    
    // Create OpenGL objects
    glGenVertexArrays(1, &plot->data->grid_vao);
    glGenBuffers(1, &plot->data->grid_vbo);
    
    glBindVertexArray(plot->data->grid_vao);
    glBindBuffer(GL_ARRAY_BUFFER, plot->data->grid_vbo);
    glBufferData(GL_ARRAY_BUFFER, total_vertices * 5 * sizeof(float), vertices, GL_STATIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    
    // Color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    
    free(vertices);
    plot->data->grid_loaded = true;
}

static void cpl_build_line_data(CPLPlot* plot, const double* x, const double* y, 
                               size_t n_points, Color color, 
                               CPLColorCallback color_fn, void* user_data) {
    if (!plot || !plot->data) return;
    
    // Expand lines array if needed
    if (plot->data->num_lines >= plot->data->capacity) {
        size_t new_capacity = plot->data->capacity == 0 ? CPL_INITIAL_CAPACITY : plot->data->capacity * 2;
        CPLLine* new_lines = (CPLLine*)realloc(plot->data->lines, new_capacity * sizeof(CPLLine));
        if (!new_lines) {
            cpl_plot_error("Failed to allocate memory for lines");
            return;
        }
        plot->data->lines = new_lines;
        plot->data->capacity = new_capacity;
    }
    
    CPLLine* line = &plot->data->lines[plot->data->num_lines];
    plot->data->num_lines++;
    
    // Initialize line
    line->vbo = 0;
    line->vao = 0;
    line->num_vertices = n_points;
    line->is_loaded = false;
    line->vertices = malloc(n_points * 5 * sizeof(float)); // 5 floats per vertex
    
    if (!line->vertices) {
        cpl_plot_error("Failed to allocate memory for line vertices");
        plot->data->num_lines--;
        return;
    }
    
    // Convert data to normalized coordinates
    double x_min = plot->x_range[0];
    double x_max = plot->x_range[1];
    double y_min = plot->y_range[0];
    double y_max = plot->y_range[1];
    
    float plot_left = -1.0f + plot->data->margin;
    float plot_right = 1.0f - plot->data->margin;
    float plot_bottom = -1.0f + plot->data->margin;
    float plot_top = 1.0f - plot->data->margin;
    
    for (size_t i = 0; i < n_points; i++) {
        float x_norm = cpl_normalize_coordinate(x[i], x_min, x_max, plot_left, plot_right - plot_left);
        float y_norm = cpl_normalize_coordinate(y[i], y_min, y_max, plot_bottom, plot_top - plot_bottom);
        
        line->vertices[i * 5 + 0] = x_norm;  // x
        line->vertices[i * 5 + 1] = y_norm;  // y
        
        // Color
        if (color_fn) {
            Color dynamic_color = color_fn(x[i], user_data);
            line->vertices[i * 5 + 2] = dynamic_color.r;
            line->vertices[i * 5 + 3] = dynamic_color.g;
            line->vertices[i * 5 + 4] = dynamic_color.b;
        } else {
            line->vertices[i * 5 + 2] = color.r;
            line->vertices[i * 5 + 3] = color.g;
            line->vertices[i * 5 + 4] = color.b;
        }
    }
    
    // Create OpenGL objects
    glGenVertexArrays(1, &line->vao);
    glGenBuffers(1, &line->vbo);
    
    glBindVertexArray(line->vao);
    glBindBuffer(GL_ARRAY_BUFFER, line->vbo);
    glBufferData(GL_ARRAY_BUFFER, n_points * 5 * sizeof(float), line->vertices, GL_STATIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    
    // Color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    line->is_loaded = true;
}

static float cpl_normalize_coordinate(double value, double min, double max, 
                                     float plot_start, float plot_size) {
    if (max == min) return plot_start + plot_size / 2.0f;
    return plot_start + (float)((value - min) / (max - min)) * plot_size;
}

static void cpl_plot_error(const char* message) {
    fprintf(stderr, "CPlotLib Error: %s\n", message);
}
