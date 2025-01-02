// CPLPlot.c

#include "CPLPlot.h"
#include "CPLFigure.h"
#include "utils/cpl_gl_utils.h"
#include "utils/cpl_renderer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <GLFW/glfw3.h>
#include <GL/glew.h>

// Structure representing a single line within a plot.
struct CPLLine {
    GLuint vbo;          // Vertex Buffer Object for the line
    GLuint vao;          // Vertex Array Object for the line
    size_t vbo_size;     // Number of vertices
    float* vertices;     // Vertex data (positions and colors)
    bool is_data_loaded; // Flag indicating if data is loaded
}; 

// Structure representing OpenGL data for a plot.
struct CPL_PLOT_GL {
    GLuint box_vbo;          // Vertex Buffer Object for the box
    GLuint box_vao;          // Vertex Array Object for the box
    size_t box_vbo_size;     // Number of vertices (4)
    float vertices[20];      // 4 vertices * (2 coords + 3 color)
    bool is_box_data_loaded; // Whether the box data is ready to be drawn

    float margin;            // Margin around the plot
};

// Forward declarations for internal helper functions.
static void initialize_gl_data(CPLPlot* plot);
static void setup_plot_box_shaders(CPLPlot* plot);
static void build_plot_box_data(CPLPlot* plot);
static void build_subplots_box_data(CPLFigure* fig, size_t rows, size_t cols, float margin);
static void setup_plot_line_shaders(CPLPlot* plot, CPLLine* line);
static void build_plot_line_data(CPLPlot* plot, double* x_arr, double* y_arr, size_t num_points, Color line_color, ColorCallback color_fn, void* user_data);

static void initialize_gl_data(CPLPlot* plot) {
    if (!plot->gl_data) {
        plot->gl_data = (CPL_PLOT_GL*)calloc(1, sizeof(CPL_PLOT_GL));
        if (!plot->gl_data) {
            fprintf(stderr, "Error: Could not allocate memory for plot->gl_data.\n");
            return;
        }
    }
}

static void generate_box_vertices(CPLPlot* plot, float left, float right, float bottom, float top) {
    float r = plot->bg_color.r;
    float g = plot->bg_color.g;
    float b = plot->bg_color.b;

    // Define vertices with positions and colors.
    float vertices[20] = {
        // x,      y,      r, g, b
        left,  top,    r, g, b,    // Vertex 0
        right, top,    r, g, b,    // Vertex 1
        right, bottom, r, g, b,    // Vertex 2
        left,  bottom, r, g, b     // Vertex 3
    };

    memcpy(plot->gl_data->vertices, vertices, sizeof(vertices));
}

static float normalize_coordinate(double value, double min, double max, float plot_start, float plot_size) {
    if (max - min == 0) {
        fprintf(stderr, "Error: Division by zero in normalization.\n");
        return plot_start;
    }
    float normalized = ((value - min) / (float)(max - min)) * plot_size + plot_start;
    // Clamp the value to ensure it stays within plot boundaries.
    if (normalized < plot_start) normalized = plot_start;
    if (normalized > plot_start + plot_size) normalized = plot_start + plot_size;
    return normalized;
}

static void setup_plot_box_shaders(CPLPlot* plot)
{
    initialize_gl_data(plot);
    if (!plot->gl_data) return;

    // Generate VAO and VBO if not already generated.
    if (plot->gl_data->box_vao == 0) glGenVertexArrays(1, &plot->gl_data->box_vao);
    if (plot->gl_data->box_vbo == 0) glGenBuffers(1, &plot->gl_data->box_vbo);

    glBindVertexArray(plot->gl_data->box_vao);
    glBindBuffer(GL_ARRAY_BUFFER, plot->gl_data->box_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plot->gl_data->vertices), plot->gl_data->vertices, GL_STATIC_DRAW);

    // Position attribute (location=0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // Color attribute (location=1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    plot->gl_data->box_vbo_size = 4;
}

static void build_plot_box_data(CPLPlot *plot)
{
    initialize_gl_data(plot);
    if (!plot->gl_data) return;

    // Set default margin if not set.
    if (plot->gl_data->margin == 0.0f) {
        plot->gl_data->margin = 0.1f; // 10% margin
    }

    float margin = plot->gl_data->margin;
    float left   = -1.0f + margin;
    float right  =  1.0f - margin;
    float bottom = -1.0f + margin;
    float top    =  1.0f - margin;

    generate_box_vertices(plot, left, right, bottom, top);
}

static void build_subplots_box_data(CPLFigure* fig, size_t rows, size_t cols, float margin)
{
    if (!fig) {
        fprintf(stderr, "Error: Figure is NULL.\n");
        return;
    }

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            size_t index = row * cols + col;
            if (index >= fig->num_plots) {
                fprintf(stderr, "Error: Plot index %zu out of range.\n", index);
                continue;
            }
            CPLPlot* plot = fig->plot[index];
            if (!plot) {
                fprintf(stderr, "Error: Plot at index %zu is NULL.\n", index);
                continue;
            }

            initialize_gl_data(plot);
            if (!plot->gl_data) return;

            plot->gl_data->margin = margin;

            // Calculate subplot dimensions based on total margins.
            float total_margin_x = margin * (cols + 1);
            float total_margin_y = margin * (rows + 1);

            float subplot_width  = (2.0f - total_margin_x) / (float)cols;
            float subplot_height = (2.0f - total_margin_y) / (float)rows;

            // Calculate subplot position in NDC.
            float left = -1.0f + margin + col * (subplot_width + margin);
            float right = left + subplot_width;
            float bottom_plot = -1.0f + margin + row * (subplot_height + margin);
            float top_plot = bottom_plot + subplot_height;

            generate_box_vertices(plot, left, right, bottom_plot, top_plot);

            // Setup the buffer/VAO for the subplot's bounding box.
            setup_plot_box_shaders(plot);

            // Mark data loaded.
            plot->gl_data->is_box_data_loaded = true;
        }
    }
}

static void setup_plot_line_shaders(CPLPlot* plot, CPLLine* line)
{
    if (!plot->gl_data || !line) {
        fprintf(stderr, "Error: plot->gl_data or line is NULL in setup_plot_line_shaders.\n");
        return;
    }

    // Generate VAO and VBO if not already generated.
    if (line->vao == 0) glGenVertexArrays(1, &line->vao);
    if (line->vbo == 0) glGenBuffers(1, &line->vbo);

    glBindVertexArray(line->vao);
    glBindBuffer(GL_ARRAY_BUFFER, line->vbo);
    glBufferData(GL_ARRAY_BUFFER, line->vbo_size * 5 * sizeof(float), line->vertices, GL_STATIC_DRAW);

    // Position attribute (location=0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // Color attribute (location=1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static void build_plot_line_data(CPLPlot* plot, double* x_arr, double* y_arr, size_t num_points, Color line_color, ColorCallback color_fn, void* user_data)
{
    if (!plot->gl_data) {
        fprintf(stderr, "Error: plot->gl_data is NULL in build_plot_line_data.\n");
        return;
    }

    // Reallocate the lines array to accommodate the new line.
    CPLLine* new_lines = (CPLLine*)realloc(plot->lines, (plot->num_lines + 1) * sizeof(CPLLine));
    if (!new_lines) {
        fprintf(stderr, "Error: Could not allocate memory for new line.\n");
        return;
    }
    plot->lines = new_lines;
    CPLLine* current_line = &plot->lines[plot->num_lines];
    plot->num_lines += 1;

    // Initialize the new line.
    current_line->vbo = 0;
    current_line->vao = 0;
    current_line->vbo_size = num_points;
    current_line->is_data_loaded = false;
    current_line->vertices = (float*)malloc(num_points * 5 * sizeof(float)); // 2 for position, 3 for color
    if (!current_line->vertices) {
        fprintf(stderr, "Error: Could not allocate memory for line vertices.\n");
        plot->num_lines -= 1;
        return;
    }

    double x_min = plot->x_range[0];
    double x_max = plot->x_range[1];
    double y_min = plot->y_range[0];
    double y_max = plot->y_range[1];

    // Prevent division by zero.
    if (x_max - x_min == 0 || y_max - y_min == 0) {
        fprintf(stderr, "Error: Invalid range for normalization.\n");
        free(current_line->vertices);
        plot->num_lines -= 1;
        return;
    }

    // Extract plot's box coordinates from gl_data->vertices.
    // Each vertex has 5 floats: x, y, r, g, b
    // Vertex 0: left, top
    // Vertex 1: right, top
    // Vertex 2: right, bottom
    // Vertex 3: left, bottom
    float plot_left = plot->gl_data->vertices[0];
    float plot_top = plot->gl_data->vertices[1];
    float plot_right = plot->gl_data->vertices[5];
    float plot_bottom = plot->gl_data->vertices[11];
    float plot_width = plot_right - plot_left;
    float plot_height = plot_top - plot_bottom;

    // Build the line data by normalizing each point.
    for (size_t i = 0; i < num_points; i++)
    {
        float normalized_x = normalize_coordinate(x_arr[i], x_min, x_max, plot_left, plot_width);
        float normalized_y = normalize_coordinate(y_arr[i], y_min, y_max, plot_bottom, plot_height); // Correctly using plot_height

        // Determine the color for this vertex.
        Color c = (color_fn) ? color_fn(x_arr[i], user_data) : line_color;

        current_line->vertices[i * 5 + 0] = normalized_x;
        current_line->vertices[i * 5 + 1] = normalized_y;
        current_line->vertices[i * 5 + 2] = c.r;
        current_line->vertices[i * 5 + 3] = c.g;
        current_line->vertices[i * 5 + 4] = c.b;
    }

    // Setup the line shaders and buffers.
    setup_plot_line_shaders(plot, current_line);

    // Mark data as loaded.
    current_line->is_data_loaded = true;
}

static void build_plot_curve_data(CPLPlot* plot, double* t_arr, double* x_arr, double* y_arr, size_t num_points, Color line_color, ColorCallback color_fn, void* user_data)
{
    if (!plot->gl_data) {
        fprintf(stderr, "Error: plot->gl_data is NULL in build_plot_line_data.\n");
        return;
    }

    // Reallocate the lines array to accommodate the new line.
    CPLLine* new_lines = (CPLLine*)realloc(plot->lines, (plot->num_lines + 1) * sizeof(CPLLine));
    if (!new_lines) {
        fprintf(stderr, "Error: Could not allocate memory for new line.\n");
        return;
    }
    plot->lines = new_lines;
    CPLLine* current_line = &plot->lines[plot->num_lines];
    plot->num_lines += 1;

    // Initialize the new line.
    current_line->vbo = 0;
    current_line->vao = 0;
    current_line->vbo_size = num_points;
    current_line->is_data_loaded = false;
    current_line->vertices = (float*)malloc(num_points * 5 * sizeof(float)); // 2 for position, 3 for color
    if (!current_line->vertices) {
        fprintf(stderr, "Error: Could not allocate memory for line vertices.\n");
        plot->num_lines -= 1;
        return;
    }

    double x_min = plot->x_range[0];
    double x_max = plot->x_range[1];
    double y_min = plot->y_range[0];
    double y_max = plot->y_range[1];

    // Prevent division by zero.
    if (x_max - x_min == 0 || y_max - y_min == 0) {
        fprintf(stderr, "Error: Invalid range for normalization.\n");
        free(current_line->vertices);
        plot->num_lines -= 1;
        return;
    }

    // Extract plot's box coordinates from gl_data->vertices.
    // Each vertex has 5 floats: x, y, r, g, b
    // Vertex 0: left, top
    // Vertex 1: right, top
    // Vertex 2: right, bottom
    // Vertex 3: left, bottom
    float plot_left = plot->gl_data->vertices[0];
    float plot_top = plot->gl_data->vertices[1];
    float plot_right = plot->gl_data->vertices[5];
    float plot_bottom = plot->gl_data->vertices[11];
    float plot_width = plot_right - plot_left;
    float plot_height = plot_top - plot_bottom;

    // Build the line data by normalizing each point.
    for (size_t i = 0; i < num_points; i++)
    {
        float normalized_x = normalize_coordinate(x_arr[i], x_min, x_max, plot_left, plot_width);
        float normalized_y = normalize_coordinate(y_arr[i], y_min, y_max, plot_bottom, plot_height); // Correctly using plot_height

        // Determine the color for this vertex.
        Color c = (color_fn) ? color_fn(t_arr[i], user_data) : line_color;

        current_line->vertices[i * 5 + 0] = normalized_x;
        current_line->vertices[i * 5 + 1] = normalized_y;
        current_line->vertices[i * 5 + 2] = c.r;
        current_line->vertices[i * 5 + 3] = c.g;
        current_line->vertices[i * 5 + 4] = c.b;
    }

    // Setup the line shaders and buffers.
    setup_plot_line_shaders(plot, current_line);

    // Mark data as loaded.
    current_line->is_data_loaded = true;
}

#pragma region Private API

void DrawPlot(CPLPlot* plot)
{
    if (!plot || !plot->gl_data || !plot->gl_data->is_box_data_loaded) {
        fprintf(stderr, "Error: Could not draw plot. Plot or data is not ready.\n");
        return;
    }

    // Build an orthographic projection: -1..1 in X, -1..1 in Y.
    float proj[16];
    makeOrthoMatrix(-1.0f, 1.0f, -1.0f, 1.0f, proj);

    // Use the figure's shader program.
    glUseProgram(plot->figure->renderer->programID);
    // Set the projection matrix uniform.
    glUniformMatrix4fv(plot->figure->renderer->proj_mat, 1, GL_FALSE, proj);

    // Draw the bounding box.
    glBindVertexArray(plot->gl_data->box_vao);
    glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)plot->gl_data->box_vbo_size);
    glBindVertexArray(0);

    // Draw each line associated with this plot.
    for (size_t i = 0; i < plot->num_lines; i++) {
        CPLLine* line = &plot->lines[i];
        if (line->is_data_loaded && line->vbo_size > 0) {
            glBindVertexArray(line->vao);
            glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)line->vbo_size);
            glBindVertexArray(0);
        }
    }

    // Cleanup: Unbind the shader program.
    glUseProgram(0);
}

void FreePlot(CPLPlot* plot)
{
    if (!plot) {
        fprintf(stderr, "Error: Could not free plot. Plot is NULL.\n");
        return;
    }

    // Clean up OpenGL objects and line data.
    if (plot->gl_data) {
        if (plot->gl_data->box_vbo)
            glDeleteBuffers(1, &plot->gl_data->box_vbo);
        if (plot->gl_data->box_vao)
            glDeleteVertexArrays(1, &plot->gl_data->box_vao);

        // Free each line's resources.
        for (size_t i = 0; i < plot->num_lines; i++) {
            CPLLine* line = &plot->lines[i];
            if (line->vbo)
                glDeleteBuffers(1, &line->vbo);
            if (line->vao)
                glDeleteVertexArrays(1, &line->vao);
            if (line->vertices)
                free(line->vertices);
        }

        // Free the lines array.
        free(plot->lines);
        plot->lines = NULL;

        // Free the OpenGL data structure.
        free(plot->gl_data);
        plot->gl_data = NULL;
    }

    // **Do not free(plot) here to avoid double free.**
}

#pragma endregion

#pragma region Public API

CPLPlot* AddPlot(CPLFigure* fig)
{
    if (!fig) {
        fprintf(stderr, "Error: Could not add plot to figure. Null pointer.\n");
        return NULL;
    }

    // Allocate memory for the new plot.
    CPLPlot* plot = (CPLPlot*)calloc(1, sizeof(CPLPlot));
    if (!plot) {
        fprintf(stderr, "Error: Could not allocate memory for plot.\n");
        return NULL;
    }

    // Reallocate the plot array to accommodate the new plot.
    CPLPlot** new_plots = (CPLPlot**)realloc(fig->plot, (fig->num_plots + 1) * sizeof(CPLPlot*));
    if (!new_plots) {
        fprintf(stderr, "Error: Could not allocate memory for plot array.\n");
        free(plot);
        return NULL;
    }
    fig->plot = new_plots;
    fig->plot[fig->num_plots] = plot;
    fig->num_plots += 1;

    // Associate the plot with the figure.
    plot->figure = fig;

    // Set default dimensions and background color.
    plot->width  = fig->width;
    plot->height = fig->height;
    plot->bg_color = COLOR_BLACK; // Default background color.

    // Build the single-plot bounding box data.
    build_plot_box_data(plot);
    setup_plot_box_shaders(plot);

    // Initialize lines array.
    plot->lines = NULL;
    plot->num_lines = 0;

    // Mark data as loaded.
    plot->gl_data->is_box_data_loaded = true;

    return plot;
}

void AddSubplots(CPLFigure* fig, size_t rows, size_t cols)
{
    if (!fig) {
        fprintf(stderr, "Error: Could not add subplots to figure. Figure is NULL.\n");
        return;
    }

    size_t total_plots = rows * cols;
    if (total_plots == 0) {
        fprintf(stderr, "Error: Number of rows and columns must be greater than zero.\n");
        return;
    }

    // Reallocate the plot array to accommodate new subplots.
    CPLPlot** new_plots = (CPLPlot**)realloc(fig->plot, (fig->num_plots + total_plots) * sizeof(CPLPlot*));
    if (!new_plots) {
        fprintf(stderr, "Error: Could not allocate memory for subplot array.\n");
        return;
    }
    fig->plot = new_plots;

    // Allocate and initialize each subplot.
    for (size_t i = 0; i < total_plots; i++) {
        CPLPlot* new_plot = (CPLPlot*)calloc(1, sizeof(CPLPlot));
        if (!new_plot) {
            fprintf(stderr, "Error: Could not allocate memory for subplot %zu.\n", i);
            // Free previously allocated subplots in this function to prevent leaks.
            for (size_t j = 0; j < i; j++) {
                FreePlot(fig->plot[fig->num_plots + j]);
                free(fig->plot[fig->num_plots + j]);
                fig->plot[fig->num_plots + j] = NULL;
            }
            fig->num_plots += (i > 0) ? i : 0;
            return;
        }

        // Associate the subplot with the figure.
        new_plot->figure = fig;

        // Set default dimensions and background color based on grid layout.
        new_plot->width  = fig->width / (float)cols;
        new_plot->height = fig->height / (float)rows;
        new_plot->bg_color = COLOR_BLACK; // Default background color.

        // Initialize lines array.
        new_plot->lines = NULL;
        new_plot->num_lines = 0;

        fig->plot[fig->num_plots + i] = new_plot;
    }

    fig->num_plots += total_plots;

    // Build each subplot's bounding box data in Normalized Device Coordinates (NDC).
    build_subplots_box_data(fig, rows, cols, 0.1f); // 10% margin between subplots.
}

CPLAPI void Plot(
    CPLPlot* plot,
    double* x_arr,
    double* y_arr,
    size_t num_points,
    Color line_color,
    ColorCallback color_fn,
    void* user_data
)
{
    if (!plot || !x_arr || !y_arr || num_points == 0) {
        fprintf(stderr, "Error: Could not plot. Invalid input.\n");
        return;
    }

    // Set default ranges if not set.
    if (plot->x_range[0] == plot->x_range[1]) {
        plot->x_range[0] = 0.0;
        plot->x_range[1] = 1.0;
    }
    if (plot->y_range[0] == plot->y_range[1]) {
        plot->y_range[0] = 0.0;
        plot->y_range[1] = 1.0;
    }

    // Build the line data.
    build_plot_line_data(plot, x_arr, y_arr, num_points, line_color, color_fn, user_data);
}

void PlotParamCurve(
    CPLPlot* plot,
    double* t_arr,
    double* x_arr,
    double* y_arr,
    size_t num_points,
    Color line_color,
    ColorCallback color_fn,
    void* user_data
)
{
    if (!plot || !t_arr || !x_arr || !y_arr || num_points == 0) {
        fprintf(stderr, "Error: Could not plot parametric curve. Invalid input.\n");
        return;
    }

    // Set default ranges if not set.
    if (plot->x_range[0] == plot->x_range[1]) {
        plot->x_range[0] = 0.0;
        plot->x_range[1] = 1.0;
    }
    if (plot->y_range[0] == plot->y_range[1]) {
        plot->y_range[0] = 0.0;
        plot->y_range[1] = 1.0;
    }

    // Build the line data.
    build_plot_curve_data(plot, t_arr, x_arr, y_arr, num_points, line_color, color_fn, user_data);
}

void SetXRange(CPLPlot* plot, double x_range[2])
{
    if (!plot || !x_range) {
        fprintf(stderr, "Error: Could not set x-range. Invalid input.\n");
        return;
    }

    memcpy(plot->x_range, x_range, 2 * sizeof(double));
}

void SetYRange(CPLPlot* plot, double y_range[2])
{
    if (!plot || !y_range) {
        fprintf(stderr, "Error: Could not set y-range. Invalid input.\n");
        return;
    }

    memcpy(plot->y_range, y_range, 2 * sizeof(double));
}

void SetZRange(CPLPlot* plot, double z_range[2])
{
    if (!plot || !z_range) {
        fprintf(stderr, "Error: Could not set z-range. Invalid input.\n");
        return;
    }

    memcpy(plot->z_range, z_range, 2 * sizeof(double));
}

#pragma endregion
