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

struct CPLLine {
    GLuint vbo;          // Vertex Buffer Object for the line
    GLuint vao;          // Vertex Array Object for the line
    size_t vbo_size;     // Number of vertices
    float* vertices;     // Vertex data (positions and colors)
    bool is_data_loaded; // Flag indicating if data is loaded
}; 

// Structure definition with added margin
struct CPL_PLOT_GL {
    GLuint box_vbo;          // Vertex Buffer Object for the box
    GLuint box_vao;          // Vertex Array Object for the box
    size_t box_vbo_size;     // Number of vertices (4)
    float vertices[20];      // 4 vertices * (2 coords + 3 color)
    bool is_box_data_loaded; // Whether the box data is ready to be drawn

    float margin;            // Margin around the plot
};

/** Forward declarations **/
static void initialize_gl_data(CPLPlot* plot);
static void setup_plot_box_shaders(CPLPlot* plot);
static void build_plot_box_data(CPLPlot *plot);
static void build_subplots_box_data(CPLFigure* fig, size_t rows, size_t cols, float margin);

static void setup_plot_line_shaders(CPLPlot* plot, CPLLine* line);
static void build_plot_line_data(CPLPlot* plot, double* x_arr, double* y_arr, size_t num_points, Color line_color, ColorCallback color_fn, void* user_data);

#pragma region Helpers

/**
 * Initializes the gl_data structure for a plot if it's not already initialized.
 */
static void initialize_gl_data(CPLPlot* plot) {
    if (!plot->gl_data) {
        plot->gl_data = (CPL_PLOT_GL*)calloc(1, sizeof(CPL_PLOT_GL));
        if (!plot->gl_data) {
            fprintf(stderr, "Error: Could not allocate memory for plot->gl_data.\n");
            return;
        }
    }
}

/**
 * Generates box vertices based on the provided dimensions and background color.
 */
static void generate_box_vertices(CPLPlot* plot, float left, float right, float bottom, float top) {
    float r = plot->bg_color.r;
    float g = plot->bg_color.g;
    float b = plot->bg_color.b;

    float vertices[20] = {
        // x,      y,      r, g, b
        left,  top,    r, g, b,
        right, top,    r, g, b,
        right, bottom, r, g, b,
        left,  bottom, r, g, b
    };

    memcpy(plot->gl_data->vertices, vertices, sizeof(vertices));
}

/**
 * Normalizes data points to the plot's coordinate system.
 */
static float normalize_coordinate(double value, double min, double max, float plot_start, float plot_size) {
    if (max - min == 0) {
        fprintf(stderr, "Error: Division by zero in normalization.\n");
        return plot_start;
    }
    float normalized = ((value - min) / (float)(max - min)) * plot_size + plot_start;
    // Clamp the value
    if (normalized < plot_start) normalized = plot_start;
    if (normalized > plot_start + plot_size) normalized = plot_start + plot_size;
    return normalized;
}

static void setup_plot_box_shaders(CPLPlot* plot)
{
    initialize_gl_data(plot);
    if (!plot->gl_data) return;

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

    // Set default margin if not set
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
            CPLPlot* plot = fig->plot[index];
            if (!plot) {
                fprintf(stderr, "Error: Plot at index %zu is NULL.\n", index);
                continue;
            }

            initialize_gl_data(plot);
            if (!plot->gl_data) return;

            plot->gl_data->margin = margin;

            // Calculate subplot dimensions
            float total_margin_x = margin * (cols + 1);
            float total_margin_y = margin * (rows + 1);

            float subplot_width  = (2.0f - total_margin_x) / (float)cols;
            float subplot_height = (2.0f - total_margin_y) / (float)rows;

            // Calculate subplot position
            float left = -1.0f + margin + col * (subplot_width + margin);
            float right = left + subplot_width;
            float bottom = -1.0f + margin + row * (subplot_height + margin);
            float top = bottom + subplot_height;

            generate_box_vertices(plot, left, right, bottom, top);

            // Setup the buffer/VAO
            setup_plot_box_shaders(plot);

            // Mark data loaded
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

    // Reallocate the lines array to accommodate the new line
    CPLLine* new_lines = (CPLLine*)realloc(plot->lines, (plot->num_lines + 1) * sizeof(CPLLine));
    if (!new_lines) {
        fprintf(stderr, "Error: Could not allocate memory for new line.\n");
        return;
    }
    plot->lines = new_lines;
    CPLLine* current_line = &plot->lines[plot->num_lines];
    plot->num_lines += 1;

    // Initialize the new line
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

    // Prevent division by zero
    if (x_max - x_min == 0 || y_max - y_min == 0) {
        fprintf(stderr, "Error: Invalid range for normalization.\n");
        free(current_line->vertices);
        plot->num_lines -= 1;
        return;
    }

    // Retrieve margin and compute plot dimensions
    float margin = plot->gl_data->margin;
    float plot_width = 2.0f - 2.0f * margin; // e.g., 1.8f
    float plot_left = -1.0f + margin;        // e.g., -0.9f
    float plot_bottom = -1.0f + margin;      // e.g., -0.9f

    // Build the line data
    for (size_t i = 0; i < num_points; i++)
    {
        float normalized_x = normalize_coordinate(x_arr[i], x_min, x_max, plot_left, plot_width);
        float normalized_y = normalize_coordinate(y_arr[i], y_min, y_max, plot_bottom, plot_width); // Assuming square plot

        Color c = (color_fn) ? color_fn(x_arr[i], user_data) : line_color;
        current_line->vertices[i * 5 + 0] = normalized_x;
        current_line->vertices[i * 5 + 1] = normalized_y;
        current_line->vertices[i * 5 + 2] = c.r;
        current_line->vertices[i * 5 + 3] = c.g;
        current_line->vertices[i * 5 + 4] = c.b;
    }

    // Setup the line shaders
    setup_plot_line_shaders(plot, current_line);

    // Mark data loaded
    current_line->is_data_loaded = true;
}

#pragma endregion

#pragma region Private API

void DrawPlot(CPLPlot* plot)
{
    if (!plot || !plot->gl_data || !plot->gl_data->is_box_data_loaded) {
        fprintf(stderr, "Error: Could not draw plot. Plot or data is not ready.\n");
        return;
    }

    // Build an orthographic projection: -1..1 in X, -1..1 in Y
    float proj[16];
    makeOrthoMatrix(-1.0f, 1.0f, -1.0f, 1.0f, proj);

    // Use the figure's shader program
    glUseProgram(plot->figure->renderer->programID);
    // Set the projection matrix
    glUniformMatrix4fv(plot->figure->renderer->proj_mat, 1, GL_FALSE, proj);

    // Draw the box
    glBindVertexArray(plot->gl_data->box_vao);
    glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)plot->gl_data->box_vbo_size);
    glBindVertexArray(0);

    // Draw each line
    for (size_t i = 0; i < plot->num_lines; i++) {
        CPLLine* line = &plot->lines[i];
        if (line->is_data_loaded && line->vbo_size > 0) {
            glBindVertexArray(line->vao);
            glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)line->vbo_size);
            glBindVertexArray(0);
        }
    }

    // Cleanup
    glUseProgram(0);
}

void FreePlot(CPLPlot* plot)
{
    if (!plot) {
        fprintf(stderr, "Error: Could not free plot. Plot is NULL.\n");
        return;
    }

    // Clean up GL objects and line data
    if (plot->gl_data) {
        if (plot->gl_data->box_vbo)
            glDeleteBuffers(1, &plot->gl_data->box_vbo);
        if (plot->gl_data->box_vao)
            glDeleteVertexArrays(1, &plot->gl_data->box_vao);

        // Free each line's resources
        for (size_t i = 0; i < plot->num_lines; i++) {
            CPLLine* line = &plot->lines[i];
            if (line->vbo)
                glDeleteBuffers(1, &line->vbo);
            if (line->vao)
                glDeleteVertexArrays(1, &line->vao);
            if (line->vertices)
                free(line->vertices);
        }

        // Free the lines array
        free(plot->lines);
        plot->lines = NULL;

        free(plot->gl_data);
        plot->gl_data = NULL;
    }

    // **Do not free(plot) here to avoid double free**
}

#pragma endregion

#pragma region Public API

CPLPlot* AddPlot(CPLFigure* fig)
{
    if (!fig) {
        fprintf(stderr, "Error: Could not add plot to figure. Null pointer.\n");
        return NULL;
    }

    // Allocate memory for the plot
    CPLPlot* plot = (CPLPlot*)calloc(1, sizeof(CPLPlot));
    if (!plot) {
        fprintf(stderr, "Error: Could not allocate memory for plot.\n");
        return NULL;
    }

    // Reallocate the plot array to accommodate the new plot
    CPLPlot** new_plots = (CPLPlot**)realloc(fig->plot, (fig->num_plots + 1) * sizeof(CPLPlot*));
    if (!new_plots) {
        fprintf(stderr, "Error: Could not allocate memory for plot array.\n");
        free(plot);
        return NULL;
    }
    fig->plot = new_plots;
    fig->plot[fig->num_plots] = plot;
    fig->num_plots += 1;

    // Associate the plot with the figure
    plot->figure = fig;

    // Set some defaults
    plot->width  = fig->width;
    plot->height = fig->height;
    plot->bg_color = COLOR_BLACK; // example

    // Build the single-plot box data
    build_plot_box_data(plot);
    setup_plot_box_shaders(plot);

    // Initialize lines array
    plot->lines = NULL;
    plot->num_lines = 0;

    // Mark data loaded
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
    CPLPlot** new_plots = (CPLPlot**)realloc(fig->plot, (fig->num_plots + total_plots) * sizeof(CPLPlot*));
    if (!new_plots) {
        fprintf(stderr, "Error: Could not allocate memory for subplot array.\n");
        return;
    }
    fig->plot = new_plots;

    for (size_t i = 0; i < total_plots; i++) {
        CPLPlot* new_plot = (CPLPlot*)calloc(1, sizeof(CPLPlot));
        if (!new_plot) {
            fprintf(stderr, "Error: Could not allocate memory for subplot %zu.\n", i);
            // Free previously allocated plots in this function
            for (size_t j = 0; j < i; j++) {
                FreePlot(fig->plot[fig->num_plots + j]);
                free(fig->plot[fig->num_plots + j]);
                fig->plot[fig->num_plots + j] = NULL;
            }
            fig->num_plots += (i > 0) ? i : 0;
            return;
        }

        // Initialize the new plot
        new_plot->figure = fig;                         // Associate with figure
        new_plot->width  = fig->width / cols;
        new_plot->height = fig->height / rows;
        new_plot->bg_color = COLOR_BLACK;               // Default background color

        // Initialize lines array
        new_plot->lines = NULL;
        new_plot->num_lines = 0;

        fig->plot[fig->num_plots + i] = new_plot;
    }

    fig->num_plots += total_plots;

    // Build each subplot's box data in NDC
    build_subplots_box_data(fig, rows, cols, 0.1f);
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

    // Set default ranges if not set
    if (plot->x_range[0] == plot->x_range[1]) {
        plot->x_range[0] = 0.0;
        plot->x_range[1] = 1.0;
    }
    if (plot->y_range[0] == plot->y_range[1]) {
        plot->y_range[0] = 0.0;
        plot->y_range[1] = 1.0;
    }

    // Build the line data
    build_plot_line_data(plot, x_arr, y_arr, num_points, line_color, color_fn, user_data);
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
