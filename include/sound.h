#ifndef __SOUND_H__
#define __SOUND_H__

#include "vector.h"
#include "color.h"
#include "string.h"
#include <stdbool.h>
#include <SDL2/SDL_mixer.h>

// wav file libraries: https://freesound.org/ (make account), https://www.zapsplat.com/ (make account),
// https://bigsoundbank.com/, https://soundbible.com/

typedef struct sound
{
    char *sound_filename;
    Mix_Chunk *chunk;
    int loops;
    bool removed;
} sound_t;

/**
 * @brief Create a new sound object
 *
 * @param sound_name filename of sound?
 * - NOTE: We malloc the sound for you, so you can declare sound on the stack. :)
 * @param position the centroid
 * @param size the dimensions of the sound rectangle
 * @return image_t*
 */
sound_t *sound_init(char *sound_filename, int loops);

/**
 * @brief Frees the sound object
 *
 * @param sound the sound object to free
 */
void sound_free(sound_t *sound);

/**
 * @brief Plays the sound
 *
 * @param sound the sound object to play
 */
void sound_play(sound_t *sound);

/**
 * @brief Mark sound to be remove from scene
 *
 * @param sound
 */
void sound_remove(sound_t *sound);

/**
 * @brief Check if the sound has been marked to be removed
 *
 * @param sound
 * @return true
 * @return false
 */
bool sound_is_removed(sound_t *sound);

#endif