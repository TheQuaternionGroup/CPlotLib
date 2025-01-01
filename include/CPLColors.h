#ifndef CPL_COLORS_H
#define CPL_COLORS_H

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A structure to store a color in RGBA format.
 */
typedef struct Color
{
    float r;
    float g;
    float b;
    float a;
} Color;

typedef struct ColorHSV
{
    float h;
    float s;
    float v;
} ColorHSV;

/**
 * Converts a color from HSV to RGB.
 * @param h Hue in [0,360]
 * @param s Saturation in [0,1]
 * @param v Value in [0,1]
 * @param[out] out The resulting color in RGB format.
 */
void HSVtoRGB(float h, float s, float v, Color* out);

/**
 * Converts a color from RGB to HSV.
 * @param r Red in [0,1]
 * @param g Green in [0,1]
 * @param b Blue in [0,1]
 * @param[out] out The resulting color in HSV format.
 */
void RGBtoHSV(float r, float g, float b, ColorHSV* out);

#define COLOR_BLACK     (Color){0.0f, 0.0f, 0.0f, 1.0f}
#define COLOR_WHITE     (Color){1.0f, 1.0f, 1.0f, 1.0f}
#define COLOR_RED       (Color){1.0f, 0.0f, 0.0f, 1.0f}
#define COLOR_GREEN     (Color){0.0f, 1.0f, 0.0f, 1.0f}
#define COLOR_BLUE      (Color){0.0f, 0.0f, 1.0f, 1.0f}
#define COLOR_YELLOW    (Color){1.0f, 1.0f, 0.0f, 1.0f}
#define COLOR_MAGENTA   (Color){1.0f, 0.0f, 1.0f, 1.0f}
#define COLOR_CYAN      (Color){0.0f, 1.0f, 1.0f, 1.0f}
#define COLOR_ORANGE    (Color){1.0f, 0.5f, 0.0f, 1.0f}
#define COLOR_PURPLE    (Color){0.5f, 0.0f, 0.5f, 1.0f}
#define COLOR_PINK      (Color){1.0f, 0.5f, 0.5f, 1.0f}
#define COLOR_LIME      (Color){0.5f, 1.0f, 0.0f, 1.0f}
#define COLOR_TEAL      (Color){0.0f, 0.5f, 0.5f, 1.0f}
#define COLOR_LAVENDER  (Color){0.5f, 0.0f, 1.0f, 1.0f}
#define COLOR_BROWN     (Color){0.6f, 0.3f, 0.0f, 1.0f}
#define COLOR_BEIGE     (Color){0.9f, 0.9f, 0.7f, 1.0f}
#define COLOR_MAROON    (Color){0.5f, 0.0f, 0.0f, 1.0f}
#define COLOR_MINT      (Color){0.0f, 0.5f, 0.3f, 1.0f}
#define COLOR_OLIVE     (Color){0.5f, 0.5f, 0.0f, 1.0f}
#define COLOR_CORAL     (Color){1.0f, 0.5f, 0.3f, 1.0f}
#define COLOR_NAVY      (Color){0.0f, 0.0f, 0.5f, 1.0f}
#define COLOR_TURQUOISE (Color){0.0f, 0.5f, 0.5f, 1.0f}
#define COLOR_SILVER    (Color){0.75f, 0.75f, 0.75f, 1.0f}
#define COLOR_GOLD      (Color){1.0f, 0.84f, 0.0f, 1.0f}
#define COLOR_SKYBLUE   (Color){0.53f, 0.81f, 0.98f, 1.0f}
#define COLOR_VIOLET    (Color){0.93f, 0.51f, 0.93f, 1.0f}
#define COLOR_INDIGO    (Color){0.29f, 0.0f, 0.51f, 1.0f}
#define COLOR_AQUAMARINE (Color){0.5f, 1.0f, 0.83f, 1.0f}
#define COLOR_TAN       (Color){0.82f, 0.71f, 0.55f, 1.0f}
#define COLOR_LINEN     (Color){0.98f, 0.94f, 0.9f, 1.0f}
#define COLOR_IVORY     (Color){1.0f, 1.0f, 0.94f, 1.0f}
#define COLOR_SNOW      (Color){1.0f, 0.98f, 0.98f, 1.0f}
#define COLOR_SEASHELL  (Color){1.0f, 0.96f, 0.93f, 1.0f}
#define COLOR_LAVENDERBLUSH (Color){1.0f, 0.94f, 0.96f, 1.0f}
#define COLOR_FLORALWHITE (Color){1.0f, 0.98f, 0.94f, 1.0f}
#define COLOR_ALICEBLUE  (Color){0.94f, 0.97f, 1.0f, 1.0f}
#define COLOR_HONEYDEW   (Color){0.94f, 1.0f, 0.94f, 1.0f}
#define COLOR_LIGHTCYAN  (Color){0.88f, 1.0f, 1.0f, 1.0f}
#define COLOR_LIGHTYELLOW (Color){1.0f, 1.0f, 0.88f, 1.0f}
#define COLOR_LIGHTGOLDENRODYELLOW (Color){0.98f, 0.98f, 0.82f, 1.0f}
#define COLOR_PAPAYAWHIP (Color){1.0f, 0.94f, 0.84f, 1.0f}
#define COLOR_MISTYROSE  (Color){1.0f, 0.89f, 0.88f, 1.0f}
#define COLOR_LEMONCHIFFON (Color){1.0f, 0.98f, 0.8f, 1.0f}
#define COLOR_LIGHTGOLDENROD (Color){0.98f, 0.98f, 0.82f, 1.0f}
#define COLOR_LIGHTCORAL (Color){0.94f, 0.5f, 0.5f, 1.0f}
#define COLOR_SALMON (Color){0.98f, 0.5f, 0.45f, 1.0f}
#define COLOR_DARKSALMON (Color){0.91f, 0.59f, 0.48f, 1.0f}
#define COLOR_LIGHTSALMON (Color){1.0f, 0.63f, 0.48f, 1.0f}
#define COLOR_CRIMSON (Color){0.86f, 0.08f, 0.24f, 1.0f}
#define COLOR_FIREBRICK (Color){0.7f, 0.13f, 0.13f, 1.0f}
#define COLOR_DARKRED (Color){0.55f, 0.0f, 0.0f, 1.0f}

// grays
#define COLOR_DARKGRAY (Color){0.66f, 0.66f, 0.66f, 1.0f}
#define COLOR_GRAY (Color){0.75f, 0.75f, 0.75f, 1.0f}
#define COLOR_LIGHTGRAY (Color){0.83f, 0.83f, 0.83f, 1.0f}
#define COLOR_GHOSTWHITE (Color){0.97f, 0.97f, 1.0f, 1.0f}
#define COLOR_WHITESMOKE (Color){0.96f, 0.96f, 0.96f, 1.0f}
#define COLOR_GAINSBORO (Color){0.86f, 0.86f, 0.86f, 1.0f}
#define COLOR_FLORALWHITE (Color){1.0f, 0.98f, 0.94f, 1.0f}
#define COLOR_OLDLACE (Color){0.99f, 0.96f, 0.9f, 1.0f}
#define COLOR_LINEN (Color){0.98f, 0.94f, 0.9f, 1.0f}
#define COLOR_ANTIQUEWHITE (Color){0.98f, 0.92f, 0.84f, 1.0f}
#define COLOR_PAPAYAWHIP (Color){1.0f, 0.94f, 0.84f, 1.0f}
#define COLOR_BLANCHEDALMOND (Color){1.0f, 0.92f, 0.8f, 1.0f}
#define COLOR_BISQUE (Color){1.0f, 0.89f, 0.77f, 1.0f}
#define COLOR_PEACHPUFF (Color){1.0f, 0.85f, 0.73f, 1.0f}
#define COLOR_NAVAJOWHITE (Color){1.0f, 0.87f, 0.68f, 1.0f}

#ifdef __cplusplus
}
#endif

#endif // CPL_COLORS_H