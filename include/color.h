#ifndef __COLOR_H__
#define __COLOR_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

/**
 * A color to display on the screen.
 * The color is represented by its red, green, and blue components.
 * Each component must be between 0 (black) and 1 (white).
 */
typedef struct
{
    float r;
    float g;
    float b;
} rgb_color_t;

extern const rgb_color_t RED;
extern const rgb_color_t GREEN;
extern const rgb_color_t BLUE;
extern const rgb_color_t BLACK;
extern const rgb_color_t WHITE;

/**
 * @brief Creates a rgb_color_t based on the index color of the rainbow.
 *
 * @param index the index to determine which color of the rainbow
 * @return a rgb_color_t based on the index color of the rainbow
 */
rgb_color_t color_rainbow(size_t index);

/**
 * @brief Creates a rgb_color_t with a randomly generated color.
 *
 * @return a rgb_color_t with a randomly generated color
 */
rgb_color_t color_random();

/**
 * @brief Increases the tint (whiteness) on a color
 *
 * @return the same color but tinted to be lighter
 */
rgb_color_t color_tint(rgb_color_t color);

/**
 * @brief Increases the shade on a color
 *
 * @param color to increase shade of 
 * @return new color
 */
rgb_color_t color_shade(rgb_color_t color);

#endif // #ifndef __COLOR_H__
