#ifndef __TEXT_H__
#define __TEXT_H__

#include "text.h"
#include "vector.h"
#include "color.h"
#include "string.h"
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>

typedef struct text
{
    char *txt;
    vector_t position;
    vector_t size;
    rgb_color_t color;
    TTF_Font *font;
    bool removed;
} text_t;

/**
 * @brief Create a new text object
 *
 * @param txt the alphanumeric text.
 * - NOTE: We malloc the text for you, so you can declare txt on the stack. :)
 * @param position the centroid
 * @param size the dimensions of the text rectangle
 * @param color the color of the font
 * @param font the font
 * @return text_t*
 */
text_t *text_init(char *txt, vector_t position, vector_t size, rgb_color_t color, TTF_Font *font);

/**
 * @brief Frees the text object
 *
 * @param text the text object to free
 */
void text_free(text_t *text);

/**
 * @brief Sets the text of the text object
 *
 * @param text the text object
 * @param new_txt the new text.
 * -- NOTE: the new_txt does not have to be on the heap since we copy the contents for you. :)
 */
void text_set(text_t *text, char *new_txt);

/**
 * @brief Sets the centroid of the text
 *
 * @param text the text object
 * @param position the new position
 */
void text_set_position(text_t *text, vector_t position);

/**
 * @brief Sets the color
 *
 * @param text the text object
 * @param color the new color
 */
void text_set_color(text_t *text, rgb_color_t color);

/**
 * @brief Sets the size of the text
 *
 * @param text the text object
 * @param size the new dimensions
 */
void text_set_size(text_t *text, vector_t size);

/**
 * @brief Sets the font of the text
 *
 * @param text the text object
 * @param size the new font
 */
void text_set_font(text_t *text, TTF_Font *font);

/**
 * @brief Mark text to be remove from scene
 *
 * @param text
 */
void text_remove(text_t *text);

/**
 * @brief Check if the text has been marked to be removed
 *
 * @param text
 * @return true
 * @return false
 */
bool text_is_removed(text_t *text);

#endif