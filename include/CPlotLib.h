/**
 * @file CPlotLib.h
 * @brief Main CPlotLib header - includes all necessary components
 * @version 2.0.0
 * @author CPlotLib Team
 * 
 * This is the main header file for CPlotLib, a high-performance plotting library
 * built on OpenGL. It provides both C and C++ APIs for creating scientific plots,
 * subplots, and data visualizations.
 */

#ifndef CPLOTLIB_H
#define CPLOTLIB_H

// Core components
#include "CPLColors.h"  // Color definitions and utilities
#include "CPLPlot.h"    // Plot structures and API

// Convenience macros for common operations
#define CPL_CREATE_FIGURE(w, h) cpl_create_figure(w, h)
#define CPL_ADD_PLOT(fig) cpl_add_plot(fig)
#define CPL_SHOW_FIGURE(fig) cpl_show_figure(fig)
#define CPL_FREE_FIGURE(fig) cpl_free_figure(fig)

// Version information
#define CPL_VERSION_MAJOR 2
#define CPL_VERSION_MINOR 0
#define CPL_VERSION_PATCH 0
#define CPL_VERSION_STRING "2.0.0"

#endif // CPLOTLIB_H
