#include "CPLColors.h"

void HSVtoRGB(float h, float s, float v, Color* out)
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

void RGBtoHSV(float r, float g, float b, ColorHSV* out)
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