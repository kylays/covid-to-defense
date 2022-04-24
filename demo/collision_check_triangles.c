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

#define DEMO_WINDOW_WIDTH 1000
#define DEMO_WINDOW_HEIGHT 1500

// THREE TRIANGLES COLLISION CHECK FOR TESTING
const double DT = 0.1;
const double V = 1.23 * 400 * 2;
const double SEPARATION_AT_COLLISION = 1.5 * 100;
const int TICKS_TO_COLLISION = 2;

body_t *make_triangle_body(double mass);

body_t *make_triangle_body(double mass)
{
    list_t *shape = list_init(3, free);
    vector_t *v = malloc(sizeof(vector_t));
    *v = (vector_t){1 * 10, 0};
    list_add(shape, v);
    v = malloc(sizeof(vector_t));
    *v = (vector_t){-0.5 * 10, +sqrt(3) / 2 * 10};
    list_add(shape, v);
    v = malloc(sizeof(vector_t));
    *v = (vector_t){-0.5 * 10, -sqrt(3) / 2 * 10};
    list_add(shape, v);
    return body_init(shape, mass, (rgb_color_t){1, 0, 0});
}

int main()
{
    vector_t translation = (vector_t){.x = WINDOW_WIDTH / 2, .y = WINDOW_WIDTH / 3};

    sdl_init(VEC_ZERO, (vector_t){.x = WINDOW_WIDTH, .y = WINDOW_HEIGHT});
    scene_t *scene = scene_init();
    body_t *body1 = make_triangle_body(1);
    vector_t initial_separation =
        {SEPARATION_AT_COLLISION + V * DT * (TICKS_TO_COLLISION - 0.5), 0};
    body_set_centroid(body1, vec_negate(initial_separation));
    body_set_velocity(body1, (vector_t){+V, 0});
    scene_add_body(scene, body1);

    body_t *body2 = make_triangle_body(INFINITY);
    scene_add_body(scene, body2);

    body_t *body3 = make_triangle_body(1);
    body_set_velocity(body3, (vector_t){-V, 0});
    body_set_centroid(body3, initial_separation);
    scene_add_body(scene, body3);

    // relative shifts
    body_set_centroid(body1, vec_add(body_get_centroid(body1), translation));
    body_set_centroid(body2, vec_add(body_get_centroid(body2), translation));
    body_set_centroid(body3, vec_add(body_get_centroid(body3), translation));

    create_physics_collision(scene, 0.3, body1, body2);
    create_physics_collision(scene, 0.3, body1, body3);
    create_physics_collision(scene, 0.3, body2, body3);
    while (!sdl_is_done(scene))
    {
        double dt = time_since_last_tick();

        // DRAW
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }

    scene_free(scene);
    return 0;
}
