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
static void HSVtoRGB(float h, float s, float v, Color* out)
{
    if (s == 0.0f)
    {
        out->r = out->g = out->b = v;
        return;
    }

    h /= 60.0f;
    int i = (int)h;
    float f = h - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    switch (i)
    {
    case 0:
        out->r = v;
        out->g = t;
        out->b = p;
        break;
    case 1:
        out->r = q;
        out->g = v;
        out->b = p;
        break;
    case 2:
        out->r = p;
        out->g = v;
        out->b = t;
        break;
    case 3:
        out->r = p;
        out->g = q;
        out->b = v;
        break;
    case 4:
        out->r = t;
        out->g = p;
        out->b = v;
        break;
    default:
        out->r = v;
        out->g = p;
        out->b = q;
        break;
    }
}

/**
 * Converts a color from RGB to HSV.
 * @param r Red in [0,1]
 * @param g Green in [0,1]
 * @param b Blue in [0,1]
 * @param[out] out The resulting color in HSV format.
 */
static void RGBtoHSV(float r, float g, float b, ColorHSV* out)
{
    float max = fmaxf(r, fmaxf(g, b));
    float min = fminf(r, fminf(g, b));
    float delta = max - min;

    out->v = max;
    if (max > 0.0f)
        out->s = delta / max;
    else
    {
        out->s = 0.0f;
        out->h = -1.0f;
        return;
    }

    if (r >= max)
        out->h = (g - b) / delta;
    else if (g >= max)
        out->h = 2.0f + (b - r) / delta;
    else
        out->h = 4.0f + (r - g) / delta;

    out->h *= 60.0f;
    if (out->h < 0.0f)
        out->h += 360.0f;
}

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

#ifdef __cplusplus
}
#endif

#endif // CPL_COLORS_H