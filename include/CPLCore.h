#ifndef CPL_CORE_H
#define CPL_CORE_H

//--------------------------------
// Core definitions
//--------------------------------

// define bool type
#define bool unsigned char
#define true 1
#define false 0

// Set up the API for exporting functions
#if defined(_MSC_VER)
    #ifdef CPL_EXPORTS
        #define CPLAPI __declspec(dllexport)
    #else
        #define CPLAPI __declspec(dllimport)
    #endif
#elif defined(__APPLE__) || defined(__linux__)
    #ifdef CPL_EXPORTS
        #define CPLAPI __attribute__((visibility("default")))
    #else
        #define CPLAPI
    #endif
#else
    #define CPLAPI
#endif

//--------------------------------
// Structs
//--------------------------------
typedef struct CPLRenderer CPLRenderer;
typedef struct CPLFigure CPLFigure;
typedef struct CPLPlot CPLPlot;


#endif // CPL_CORE_H