#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "list.h"
#include "color.h"
#include "body.h"
#include "vector.h"
#include "sdl_wrapper.h"
#include "scene.h"
#include "forces.h"

// FORCE CONFIGURATION
const double SPRING_CONSTANT_K = 5;
const double DRAG_GAMMA = 2;

// SPRING CONFIGURATION
const double SPRING_NUM_BALLS = WINDOW_WIDTH / (2 * 10);
const double SPRING_BALL_RADIUS = 10;
const size_t SPRING_BALL_NUM_POINTS = 20;
const double SPRING_BALL_MASS = 5;
const rgb_color_t SPRING_BALL_START_COLOR = {.r = 1, .b = 0, .g = 0};
const rgb_color_t SPRING_BALL_INVISIBLE_COLOR = {.r = 1, .b = 1, .g = 1};
const double SPRING_AMP_RATIO = 35.0;

// FUNCTION DECLARATIONS
void make_spring_ball(scene_t *scene, vector_t *position, rgb_color_t color);
void make_spring(scene_t *scene);
void reset_spring(scene_t *scene);
void damping_key_handler(char key, key_event_type_t type, double held_time, void *obj);

// FUNCTIONS
void make_spring_ball(scene_t *scene, vector_t *position, rgb_color_t color)
{
    list_t *shape = list_init(SPRING_BALL_NUM_POINTS, free);
    for (double i = 0; i < 2 * M_PI; i += 2 * M_PI / SPRING_BALL_NUM_POINTS)
    {
        double x = SPRING_BALL_RADIUS * cos(i);
        double y = SPRING_BALL_RADIUS * sin(i);

        vector_t *point = malloc(sizeof(vector_t));
        point->x = position->x + x;
        point->y = position->y + y;

        list_add(shape, point);
    }

    body_t *spring_ball = body_init(shape, SPRING_BALL_MASS, color);
    body_set_centroid(spring_ball, *position);
    scene_add_body(scene, spring_ball);
}

void make_spring(scene_t *scene)
{
    double amplitude = WINDOW_HEIGHT / 2;
    double x = SPRING_BALL_RADIUS;
    rgb_color_t color = SPRING_BALL_START_COLOR;

    for (int i = 0; i < SPRING_NUM_BALLS; i++)
    {
        vector_t *position_invisible = malloc(sizeof(vector_t));
        *position_invisible = (vector_t){.x = x, .y = WINDOW_HEIGHT / 2 - amplitude};
        make_spring_ball(scene, position_invisible, SPRING_BALL_INVISIBLE_COLOR);
        create_drag(scene, DRAG_GAMMA, scene_get_body(scene, 2 * i));

        vector_t *position_visible = malloc(sizeof(vector_t));
        *position_visible = (vector_t){.x = x, .y = WINDOW_HEIGHT / 2 + amplitude};
        color = color_rainbow(i);
        make_spring_ball(scene, position_visible, color);
        create_drag(scene, DRAG_GAMMA, scene_get_body(scene, 2 * i + 1));

        create_spring(scene, SPRING_CONSTANT_K, scene_get_body(scene, 2 * i),
                      scene_get_body(scene, 2 * i + 1));

        x += SPRING_BALL_RADIUS * 2;
        amplitude = (WINDOW_HEIGHT / 2) * pow(M_E, -i / SPRING_AMP_RATIO) * cos(2 * M_PI * i / SPRING_AMP_RATIO);
    }
}

void reset_spring(scene_t *scene)
{
    double amplitude = WINDOW_HEIGHT / 2;
    double x = SPRING_BALL_RADIUS;

    for (int i = 0; i < SPRING_NUM_BALLS; i++)
    {
        vector_t *position_invisible = malloc(sizeof(vector_t));
        *position_invisible = (vector_t){.x = x, .y = WINDOW_HEIGHT / 2 - amplitude};
        body_set_centroid(scene_get_body(scene, 2 * i), *position_invisible);

        vector_t *position_visible = malloc(sizeof(vector_t));
        *position_visible = (vector_t){.x = x, .y = WINDOW_HEIGHT / 2 + amplitude};
        body_set_centroid(scene_get_body(scene, 2 * i + 1), *position_visible);

        x += SPRING_BALL_RADIUS * 2;
        amplitude = (WINDOW_HEIGHT / 2) * pow(M_E, -i / SPRING_AMP_RATIO) * cos(2 * M_PI * i / SPRING_AMP_RATIO);
    }
}

void damping_key_handler(char key, key_event_type_t type, double held_time, void *obj)
{
    scene_t *scene = (scene_t *)obj;

    if (type == KEY_PRESSED && key == UP_ARROW)
    {
        reset_spring(scene);
    }
    else if (type == KEY_PRESSED && key == ESCAPE)
    {
        printf("ESC pressed\n");
        exit(1);
    }
}

int main()
{
    sdl_init(VEC_ZERO, (vector_t){.x = WINDOW_WIDTH, .y = WINDOW_HEIGHT});
    scene_t *scene = scene_init();
    make_spring(scene);
    sdl_on_key(damping_key_handler);

    while (!sdl_is_done(scene))
    {
        double dt = time_since_last_tick();
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }

    scene_free(scene);
}
