#include "CPlotLib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Benchmark configuration
#define BENCHMARK_POINTS 100000
#define BENCHMARK_ITERATIONS 100
#define BENCHMARK_PLOTS 10

// Benchmark results
typedef struct {
    double setup_time;
    double render_time;
    double total_time;
    size_t points_processed;
    size_t plots_created;
} BenchmarkResult;

// Generate test data
void generate_test_data(double* x, double* y, size_t n_points) {
    for (size_t i = 0; i < n_points; i++) {
        double t = (double)i / (n_points - 1) * 4 * M_PI;
        x[i] = t;
        y[i] = sin(t) + 0.1 * sin(10 * t);
    }
}

// Benchmark plotting performance
BenchmarkResult benchmark_plotting(size_t n_points, size_t n_plots) {
    BenchmarkResult result = {0};
    
    clock_t start_total = clock();
    
    // Create figure
    clock_t start_setup = clock();
    CPLFigure* fig = cpl_create_figure(1200, 800);
    if (!fig) {
        printf("Failed to create figure\n");
        return result;
    }
    
    // Create plots
    CPLPlot** plots = malloc(n_plots * sizeof(CPLPlot*));
    for (size_t i = 0; i < n_plots; i++) {
        plots[i] = cpl_add_plot(fig);
        if (plots[i]) {
            cpl_set_x_range(plots[i], 0, 4 * M_PI);
            cpl_set_y_range(plots[i], -2, 2);
            cpl_show_grid(plots[i], true);
        }
    }
    
    // Generate test data
    double* x = malloc(n_points * sizeof(double));
    double* y = malloc(n_points * sizeof(double));
    generate_test_data(x, y, n_points);
    
    clock_t end_setup = clock();
    result.setup_time = ((double)(end_setup - start_setup)) / CLOCKS_PER_SEC;
    
    // Benchmark plotting
    clock_t start_render = clock();
    for (size_t i = 0; i < n_plots; i++) {
        if (plots[i]) {
            Color colors[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN};
            cpl_plot(plots[i], x, y, n_points, colors[i % 5], NULL, NULL);
        }
    }
    clock_t end_render = clock();
    result.render_time = ((double)(end_render - start_render)) / CLOCKS_PER_SEC;
    
    clock_t end_total = clock();
    result.total_time = ((double)(end_total - start_total)) / CLOCKS_PER_SEC;
    result.points_processed = n_points * n_plots;
    result.plots_created = n_plots;
    
    // Cleanup
    free(plots);
    free(x);
    free(y);
    cpl_free_figure(fig);
    
    return result;
}

// Benchmark subplot performance
BenchmarkResult benchmark_subplots(size_t rows, size_t cols) {
    BenchmarkResult result = {0};
    
    clock_t start_total = clock();
    
    // Create figure with subplots
    clock_t start_setup = clock();
    CPLFigure* fig = cpl_create_figure(1200, 800);
    if (!fig) {
        printf("Failed to create figure\n");
        return result;
    }
    
    cpl_add_subplots(fig, rows, cols);
    
    // Generate test data
    double* x = malloc(BENCHMARK_POINTS * sizeof(double));
    double* y = malloc(BENCHMARK_POINTS * sizeof(double));
    generate_test_data(x, y, BENCHMARK_POINTS);
    
    clock_t end_setup = clock();
    result.setup_time = ((double)(end_setup - start_setup)) / CLOCKS_PER_SEC;
    
    // Plot data in each subplot
    clock_t start_render = clock();
    size_t total_plots = rows * cols;
    for (size_t i = 0; i < total_plots; i++) {
        CPLPlot* plot = cpl_get_subplot(fig, i);
        if (plot) {
            cpl_set_x_range(plot, 0, 4 * M_PI);
            cpl_set_y_range(plot, -2, 2);
            cpl_show_grid(plot, true);
            
            Color colors[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_YELLOW};
            cpl_plot(plot, x, y, BENCHMARK_POINTS, colors[i % 6], NULL, NULL);
        }
    }
    clock_t end_render = clock();
    result.render_time = ((double)(end_render - start_render)) / CLOCKS_PER_SEC;
    
    clock_t end_total = clock();
    result.total_time = ((double)(end_total - start_total)) / CLOCKS_PER_SEC;
    result.points_processed = BENCHMARK_POINTS * total_plots;
    result.plots_created = total_plots;
    
    // Cleanup
    free(x);
    free(y);
    cpl_free_figure(fig);
    
    return result;
}

// Print benchmark results
void print_results(const char* test_name, BenchmarkResult result) {
    printf("\n=== %s ===\n", test_name);
    printf("Setup time: %.6f seconds\n", result.setup_time);
    printf("Render time: %.6f seconds\n", result.render_time);
    printf("Total time: %.6f seconds\n", result.total_time);
    printf("Points processed: %zu\n", result.points_processed);
    printf("Plots created: %zu\n", result.plots_created);
    printf("Performance: %.0f points/second\n", result.points_processed / result.render_time);
}

int main() {
    printf("CPlotLib Performance Benchmark\n");
    printf("==============================\n");
    
    // Test 1: Single plot with many points
    BenchmarkResult result1 = benchmark_plotting(BENCHMARK_POINTS, 1);
    print_results("Single Plot Performance", result1);
    
    // Test 2: Multiple plots
    BenchmarkResult result2 = benchmark_plotting(BENCHMARK_POINTS / 10, BENCHMARK_PLOTS);
    print_results("Multiple Plots Performance", result2);
    
    // Test 3: Subplots (2x2)
    BenchmarkResult result3 = benchmark_subplots(2, 2);
    print_results("2x2 Subplots Performance", result3);
    
    // Test 4: Subplots (3x3)
    BenchmarkResult result4 = benchmark_subplots(3, 3);
    print_results("3x3 Subplots Performance", result4);
    
    printf("\nBenchmark completed successfully!\n");
    return 0;
}