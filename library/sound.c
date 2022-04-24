#include "sound.h"

sound_t *sound_init(char *sound_filename, int loops)
{
    sound_t *sound = malloc(sizeof(sound_t));
    Mix_Chunk *gScratch = Mix_LoadWAV(sound_filename);
    sound->chunk = gScratch;
    sound->loops = loops;
    return sound;
}

void sound_free(sound_t *sound)
{
    Mix_FreeChunk(sound->chunk);
    free(sound);
}

void sound_play(sound_t *sound)
{
    Mix_PlayChannel(-1, sound->chunk, sound->loops);
}

void sound_remove(sound_t *sound)
{
    sound->removed = true;
}

bool sound_is_removed(sound_t *sound)
{
    return sound->removed;
}
