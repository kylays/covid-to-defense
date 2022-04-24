#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "vector.h"
#include "color.h"
#include "string.h"
#include <stdbool.h>
#include <SDL2/SDL_image.h>

typedef struct image
{
    char *image_filename;
    vector_t position;
    vector_t size;
    bool removed;
    int image_type; // IMG_INIT_PNG or IMG_INIT_JPG
} image_t;

/**
 * @brief Create a new image object, the filename should be freed after init if not used elsewhere
 *
 * @param image_name filename of image
 * - NOTE: We malloc the image for you, so you can declare txt on the stack. :)
 * @param position the centroid
 * @param size the dimensions of the image rectangle
 * @param image_type IMG_INIT_PNG or IMG_INIT_JPG
 * @return image_t*
 */
image_t *image_init(char *image_name, vector_t position, vector_t size, int image_type);

/**
 * @brief Frees the image struct but the filename should be freed after init
 *
 * @param image the image object to free
 */
void image_free(image_t *image);

/**
 * @brief Sets the image of the image object
 *
 * @param image the image object
 * @param new_image_name the new image. 
 * -- NOTE: the new_txt does not have to be on the heap since we copy the contents for you. :)
 */
void image_set(image_t *image, char *new_image_name);

/**
 * @brief Sets the centroid of the image
 *
 * @param image the image object
 * @param position the new position
 */
void image_set_position(image_t *image, vector_t position);

/**
 * @brief Sets the size of the image
 *
 * @param image the image object
 * @param size the new dimensions
 */
void image_set_size(image_t *image, vector_t size);

/**
 * @brief Mark image to be remove from scene
 *
 * @param image to be marked for removal
 */
void image_remove(image_t *image);

/**
 * @brief Check if the image has been marked to be removed
 *
 * @param image to check
 * @return whether image has been marked to be removed
 */
bool image_is_removed(image_t *image);

#endif