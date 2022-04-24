#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>
// #include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "sdl_wrapper.h"
#include "sound.h"

// #define COMMON_COLOR_MAX 255
// #define COMMON_WINDOW_WIDTH 500
// #define COMMON_WINDOW_HEIGHT (COMMON_WINDOW_WIDTH)

int main() {

    sdl_init((vector_t) {0, 0}, (vector_t) {1500, 1000});

    scene_t *scene = scene_init();

    sound_t *sound = sound_init("sounds/pop.wav", 2);
    sound_play(sound);

    while (!sdl_is_done(NULL))
    {
        double dt = time_since_last_tick();

        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }

    sound_free(sound);

    return 1;
}
