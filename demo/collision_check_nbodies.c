#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "list.h"
#include "body.h"
#include "vector.h"
#include "sdl_wrapper.h"
#include "scene.h"
#include "forces.h"
#include "math.h"

// NBODIES COLLISION CHECK FOR TESTING
const size_t NUM_STARTING_BODIES = 20;
const size_t DIAMOND_POINTS = 8;
const int MAX_RADIUS = 30;
const int MIN_RADIUS = 3;

void add_diamond(scene_t *scene)
{
    list_t *shape = list_init(DIAMOND_POINTS, free);

    double outer_radius = rand() % MAX_RADIUS + MIN_RADIUS;
    double inner_radius = outer_radius;

    for (double i = 0; i < 2 * M_PI; i += M_PI / 2)
    {

        vector_t *outer_point = malloc(sizeof(vector_t));
        vector_t *inner_point = malloc(sizeof(vector_t));

        outer_point->x = outer_radius * cos(i);
        outer_point->y = outer_radius * sin(i);

        inner_point->x = inner_radius * cos(i + M_PI / 4);
        inner_point->y = inner_radius * sin(i + M_PI / 4);

        list_add(shape, outer_point);
        list_add(shape, inner_point);
    }

    double mass = outer_radius * 2;
    rgb_color_t color = color_random();
    vector_t coords = {.x = (double)(rand() % WINDOW_WIDTH), .y = (double)(rand() % WINDOW_HEIGHT)};

    body_t *diamond = body_init(shape, mass, color);
    body_set_centroid(diamond, coords);
    scene_add_body(scene, diamond);

    size_t num_bodies = scene_bodies(scene);

    for (int i = 0; i < num_bodies - 1; i++)
    {
        create_newtonian_gravity(scene, 1, scene_get_body(scene, i), scene_get_body(scene, num_bodies - 1));
        // create_destructive_collision(scene, scene_get_body(scene, i), scene_get_body(scene, num_bodies - 1));
        create_physics_collision(scene, 1.0, scene_get_body(scene, i), scene_get_body(scene, num_bodies - 1));
    }
}

void nbodies_key_handler(char key, key_event_type_t type, double held_time, void *obj)
{
    scene_t *scene = (scene_t *)obj;
    if (type == KEY_PRESSED && key == UP_ARROW)
    {
        add_diamond(scene);
    }
}

int main()
{
    srand(42);
    sdl_init(VEC_ZERO, (vector_t){.x = WINDOW_WIDTH, .y = WINDOW_HEIGHT});
    scene_t *scene = scene_init();
    sdl_on_key(nbodies_key_handler);

    for (int i = 0; i < NUM_STARTING_BODIES; i++)
    {
        add_diamond(scene);
    }

    while (!sdl_is_done(scene))
    {
        double dt = time_since_last_tick();

        // DRAW
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }

    // CLEANUP
    scene_free(scene);

    return 0;
}