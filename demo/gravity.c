#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "star.h"
#include "list.h"
#include "polygon.h"
#include "sdl_wrapper.h"

const size_t INITIAL_STAR_LIST_CAPACITY = 5;
const size_t INITIAL_STAR_POINTS = 2;
const double DELAY = 0.5;

int main()
{
    sdl_init(VEC_ZERO, (vector_t){.x = WINDOW_WIDTH, .y = WINDOW_HEIGHT});
    list_t *stars = list_init(INITIAL_STAR_LIST_CAPACITY, (free_func_t)star_free);
    double counter = DELAY;
    int num_stars = 0;

    while (!sdl_is_done(NULL))
    {
        if (counter >= DELAY)
        {
            counter = 0.0;
            star_t *new_star = star_init_with_n(num_stars + INITIAL_STAR_POINTS);
            list_add(stars, new_star);
            num_stars++;
        }

        sdl_clear();
        double dt = time_since_last_tick();

        size_t index = 0;
        while (index < list_size(stars))
        {

            star_t *star = list_get(stars, index);

            // UPDATE
            star_move_gravity(star, dt);

            // DRAW
            star_draw(star);
            if (star_off_screen(star))
            {
                star_free(list_remove(stars, 0));
            }
            else
            {
                index++;
            }
        }

        sdl_show();
        counter += dt;
    }
    list_free(stars);
}