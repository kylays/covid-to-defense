#include "image.h"

image_t *image_init(char *image_filename, vector_t position, vector_t size, int image_type)
{
    image_t *image = malloc(sizeof(image_t));
    *image = (image_t){.image_filename = image_filename, .position = position, 
                        .size = size, .removed = false, .image_type = image_type};
    return image;
}

void image_set(image_t *image, char *new_image_name)
{
    strcpy(image->image_filename, new_image_name);
}

void image_free(image_t *image)
{
    free(image);
}

void image_set_position(image_t *image, vector_t position)
{
    image->position = position;
}

void image_set_size(image_t *image, vector_t size)
{
    image->size = size;
}

void image_remove(image_t *image)
{
    image->removed = true;
}

bool image_is_removed(image_t *image)
{
    return image->removed;
}
