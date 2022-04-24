#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "sdl_wrapper.h"
#include "list.h"
#include "vector.h"
#include "polygon.h"
#include "star.h"

// customize star
const vector_t VELOCITY = {.x = 100.0, .y = -100.0};
const double RADIUS = 100.0;
const size_t POINTS = 5;
const double ROT_ANGLE = M_PI / 1052;
const rgb_color_t START_COLOR = {.r = 0.0, .g = 255.0, .b = 0.0};

int main(int argc, char *argv[])
{
    sdl_init(VEC_ZERO, (vector_t){.x = WINDOW_WIDTH, .y = WINDOW_HEIGHT});

    vector_t position = {.x = WINDOW_WIDTH / 2, .y = WINDOW_HEIGHT / 2};
    star_t *star = star_init(position, VELOCITY, RADIUS, POINTS, ROT_ANGLE, START_COLOR);

    while (!sdl_is_done(NULL))
    {
        sdl_clear();
        double dt = time_since_last_tick();

        // UPDATE
        star_move_bounce(star, dt);
        star_update_rainbow_color(star);

        // DRAW
        star_draw(star);
        sdl_show();
    }

    star_free(star);
}
