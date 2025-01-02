#ifndef CPL_CORE_H
#define CPL_CORE_H

//--------------------------------
// Core definitions
//--------------------------------

#ifndef __cplusplus
    // Define bool type only for C
    #ifndef bool
        #define bool unsigned char
    #endif
    #ifndef true
        #define true 1
    #endif
    #ifndef false
        #define false 0
    #endif
#endif // __cplusplus

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
#ifdef __cplusplus
extern "C" {
#endif

// For C code
typedef struct CPLRenderer CPLRenderer;
typedef struct CPLFigure CPLFigure;
typedef struct CPLPlot CPLPlot;

#ifdef __cplusplus
}
#endif

#endif // CPL_CORE_H
