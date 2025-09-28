#ifndef CPL_COLORS_H
#define CPL_COLORS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// Color structure with RGBA components
typedef struct {
    float r, g, b, a;
} Color;

// HSV color structure
typedef struct {
    float h, s, v;
} ColorHSV;

// Predefined colors
extern const Color COLOR_RED;
extern const Color COLOR_GREEN;
extern const Color COLOR_BLUE;
extern const Color COLOR_YELLOW;
extern const Color COLOR_CYAN;
extern const Color COLOR_MAGENTA;
extern const Color COLOR_WHITE;
extern const Color COLOR_BLACK;
extern const Color COLOR_GRAY;
extern const Color COLOR_LIGHTGRAY;
extern const Color COLOR_DARKGRAY;
extern const Color COLOR_ORANGE;
extern const Color COLOR_PURPLE;
extern const Color COLOR_PINK;
extern const Color COLOR_BROWN;

// Color conversion functions
void cpl_hsv_to_rgb(float h, float s, float v, Color* out);
void cpl_rgb_to_hsv(float r, float g, float b, ColorHSV* out);

#ifdef __cplusplus
}
#endif

#endif // CPL_COLORS_H