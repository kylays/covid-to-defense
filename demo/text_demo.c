#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "color.h"
#include "vector.h"
#include "text.h"
#include "sdl_wrapper.h"

#include <SDL2/SDL_ttf.h>

void update_text(text_t *text);

void update_text(text_t *text) {
    char txt[100];
    snprintf(txt, 100, "%u", (unsigned int)(time(NULL) % 1000));
    if (rand() % 30 == 1) {
        text_set_position(text, (vector_t) {.x = rand() % 1000, .y = rand() % 500});
    }
    text_set(text, txt);
}

int main(int argc, char **argv) {

    sdl_init(VEC_ZERO, (vector_t){.x = 1000, .y = 500});
    scene_t *scene = scene_init();

    TTF_Font* font = create_font("fonts/OpenSans-Bold.ttf", 100);

    vector_t position = {.x = 500, .y = 250};
    vector_t size = {.x = 150, .y = 150};
    rgb_color_t color = {.r = 0, .b = 1, .g = 0};

    text_t *text = text_init("Counter", position, size, color, font);
    text_t *text2 = text_init("Hello World", (vector_t) {.x = 500, .y = 350}, (vector_t) {.x = 300, .y = 150}, (rgb_color_t) {.r = 1, .g = 0, .b = 0}, font);

    scene_add_text(scene, text);
    scene_add_text(scene, text2);

    update_text(text);

    clock_t start = clock();

    while (!sdl_is_done(NULL)) {

        double dt = time_since_last_tick();

        scene_tick(scene, dt);
        sdl_render_scene(scene);
        update_text(text);

        if ((double) (clock() - start) / CLOCKS_PER_SEC >= 3 && scene_texts(scene) == 2) {
            printf("removed\n");
            text_remove(text2);
        }

    }
    sdl_cleanup();
    return EXIT_SUCCESS;
}
