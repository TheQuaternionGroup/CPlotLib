#include "CPLColors.h"
#include <math.h>

// Predefined colors
const Color COLOR_RED = {1.0f, 0.0f, 0.0f, 1.0f};
const Color COLOR_GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
const Color COLOR_BLUE = {0.0f, 0.0f, 1.0f, 1.0f};
const Color COLOR_YELLOW = {1.0f, 1.0f, 0.0f, 1.0f};
const Color COLOR_CYAN = {0.0f, 1.0f, 1.0f, 1.0f};
const Color COLOR_MAGENTA = {1.0f, 0.0f, 1.0f, 1.0f};
const Color COLOR_WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
const Color COLOR_BLACK = {0.0f, 0.0f, 0.0f, 1.0f};
const Color COLOR_GRAY = {0.5f, 0.5f, 0.5f, 1.0f};
const Color COLOR_LIGHTGRAY = {0.8f, 0.8f, 0.8f, 1.0f};
const Color COLOR_DARKGRAY = {0.2f, 0.2f, 0.2f, 1.0f};
const Color COLOR_ORANGE = {1.0f, 0.5f, 0.0f, 1.0f};
const Color COLOR_PURPLE = {0.5f, 0.0f, 1.0f, 1.0f};
const Color COLOR_PINK = {1.0f, 0.75f, 0.8f, 1.0f};
const Color COLOR_BROWN = {0.6f, 0.3f, 0.0f, 1.0f};

void cpl_hsv_to_rgb(float h, float s, float v, Color* out) {
    if (!out) return;
    
    if (s == 0.0f) {
        out->r = out->g = out->b = v;
        return;
    }

    h /= 60.0f;
    int i = (int)h;
    float f = h - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    switch (i) {
        case 0:
            out->r = v; out->g = t; out->b = p;
            break;
        case 1:
            out->r = q; out->g = v; out->b = p;
            break;
        case 2:
            out->r = p; out->g = v; out->b = t;
            break;
        case 3:
            out->r = p; out->g = q; out->b = v;
            break;
        case 4:
            out->r = t; out->g = p; out->b = v;
            break;
        default:
            out->r = v; out->g = p; out->b = q;
            break;
    }
}

void cpl_rgb_to_hsv(float r, float g, float b, ColorHSV* out) {
    if (!out) return;
    
    float max = fmaxf(r, fmaxf(g, b));
    float min = fminf(r, fminf(g, b));
    float delta = max - min;

    out->v = max;
    if (max > 0.0f) {
        out->s = delta / max;
    } else {
        out->s = 0.0f;
        out->h = -1.0f;
        return;
    }

    if (r >= max) {
        out->h = (g - b) / delta;
    } else if (g >= max) {
        out->h = 2.0f + (b - r) / delta;
    } else {
        out->h = 4.0f + (r - g) / delta;
    }

    out->h *= 60.0f;
    if (out->h < 0.0f) {
        out->h += 360.0f;
    }
}
