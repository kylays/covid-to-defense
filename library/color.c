#include "color.h"

const rgb_color_t RED = {.r = 1, .g = 0, .b = 0};
const rgb_color_t GREEN = {.r = 0, .g = 1, .b = 0};
const rgb_color_t BLUE = {.r = 0, .g = 0, .b = 1};
const rgb_color_t BLACK = {0, 0, 0};
const rgb_color_t WHITE = {1, 1, 1};
const float COLOR_MAX = 1.0;

// RAINBOW CONSTS
const float RAINBOW_FREQ = 0.25;
const rgb_color_t RAINBOW_PHASE = {.r = 0.0, .g = 2.0, .b = 4.0};
const size_t RAINBOW_NUM = 50;
const float RAINBOW_WIDTH = 125.0;
const float RAINBOW_CEN = 125.0;
const float RAINBOW_SCALE = 255.0;

const float TINT_FACTOR = (float)0.40;
const float SHADE_FACTOR = (float)0.2;

float compute_rainbow(float phase, size_t index);

float compute_rainbow(float phase, size_t index)
{
    return (sinf((float)(RAINBOW_FREQ * (RAINBOW_NUM - index % RAINBOW_NUM) + phase)) * RAINBOW_WIDTH + RAINBOW_CEN) / RAINBOW_SCALE;
}

rgb_color_t color_rainbow(size_t index)
{
    float r = compute_rainbow(RAINBOW_PHASE.r, index);
    float g = compute_rainbow(RAINBOW_PHASE.g, index);
    float b = compute_rainbow(RAINBOW_PHASE.b, index);
    return (rgb_color_t){.r = r, .g = g, .b = b};
}

rgb_color_t color_random()
{
    return (rgb_color_t){.r = (float)rand() / (float)RAND_MAX, .g = (float)rand() / (float)RAND_MAX, .b = (float)rand() / (float)RAND_MAX};
}

rgb_color_t color_tint(rgb_color_t color)
{
    float r = color.r + (COLOR_MAX - color.r) * TINT_FACTOR;
    float g = color.g + (COLOR_MAX - color.g) * TINT_FACTOR;
    float b = color.b + (COLOR_MAX - color.b) * TINT_FACTOR;

    r = r >= COLOR_MAX ? COLOR_MAX : r;
    g = g >= COLOR_MAX ? COLOR_MAX : g;
    b = b >= COLOR_MAX ? COLOR_MAX : b;

    rgb_color_t tint = color;

    if (color.r > color.b && color.r > color.g)
    {
        tint = (rgb_color_t){.r = color.r, .g = g, .b = b};
    }
    else if (color.g > color.b && color.g > color.r)
    {
        tint = (rgb_color_t){.r = r, .g = color.g, .b = b};
    }
    else if (color.b > color.r && color.b > color.g)
    {
        tint = (rgb_color_t){.r = r, .g = g, .b = color.b};
    }
    return tint;
}

rgb_color_t color_shade(rgb_color_t color)
{
    float r = color.r * (1 - SHADE_FACTOR);
    float g = color.g * (1 - SHADE_FACTOR);
    float b = color.b * (1 - SHADE_FACTOR);
    return (rgb_color_t){.r = r, .g = g, .b = b};
}
