#include "text.h"

const int MAX_TEXT_LENGTH = 100;

text_t *text_init(char *txt, vector_t position, vector_t size, rgb_color_t color, TTF_Font *font)
{
    text_t *text = malloc(sizeof(text_t));
    char *new_text = malloc(sizeof(char) * MAX_TEXT_LENGTH);
    strcpy(new_text, txt);
    *new_text = *txt;
    *text = (text_t){.txt = new_text, .position = position, .size = size, .color = color, .font = font, .removed = false};
    return text;
}

void text_set(text_t *text, char *new_txt)
{
    strcpy(text->txt, new_txt);
    // free(new_txt);
}

void text_free(text_t *text)
{
    free(text->txt);
    free(text);
}

void text_set_position(text_t *text, vector_t position)
{
    text->position = position;
}

void text_set_color(text_t *text, rgb_color_t color)
{
    text->color = color;
}

void text_set_font(text_t *text, TTF_Font *font)
{
    text->font = font;
}

void text_set_size(text_t *text, vector_t size)
{
    text->size = size;
}

void text_remove(text_t *text)
{
    text->removed = true;
}

bool text_is_removed(text_t *text)
{
    return text->removed;
}
