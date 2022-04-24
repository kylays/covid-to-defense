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
const double DRAG_GAMMA = 1;

// BALL CONFIGURATION
const double NUM_BALLS = WINDOW_WIDTH / 5;
const size_t BALL_NUM_POINTS = 10;
const double BALL_RADIUS = 5;
const double BALL_INVISIBLE_RADIUS = .0001;
const double BALL_MASS = 5;
const rgb_color_t BALL_START_COLOR = {.r = 1, .b = 0, .g = 0};
const rgb_color_t BALL_INVISIBLE_COLOR = {.r = 1, .b = 1, .g = 1};

// WAVY CIRCLE CONFIGURATION
const double WAVY_CIRCLE_AMPLITUDE = 50;
const double WAVY_CIRCLE_FREQUENCY = 10;
const double WAVY_CIRCLE_DEFAULT_TRANSLATION = 150;
const double WAVY_CIRCLE_SMALL_TRANSLATION = 100;
const double WAVY_CIRCLE_BIG_TRANSLATION = 200;

// FUNCTION DECLARATIONS
void make_ball(scene_t *scene, vector_t *position, double radius, rgb_color_t color);
void make_wavy_circle(scene_t *scene);
void reset_wavy_circle(scene_t *scene, double translation_change);
void wavy_circle_key_handler(char key, key_event_type_t type, double held_time, void *obj);

// FUNCTIONS
void make_ball(scene_t *scene, vector_t *position, double radius, rgb_color_t color)
{
    list_t *shape = list_init(BALL_NUM_POINTS, free);
    for (double i = 0; i < 2 * M_PI; i += 2 * M_PI / BALL_NUM_POINTS)
    {
        double x = radius * cos(i);
        double y = radius * sin(i);

        vector_t *point = malloc(sizeof(vector_t));
        point->x = position->x + x;
        point->y = position->y + y;

        list_add(shape, point);
    }

    body_t *spring_ball = body_init(shape, BALL_MASS, color);
    body_set_centroid(spring_ball, *position);
    scene_add_body(scene, spring_ball);
}

void make_wavy_circle(scene_t *scene)
{
    double theta = 0;
    rgb_color_t color = BALL_START_COLOR;

    for (int i = 0; i < NUM_BALLS; i++)
    {
        vector_t center = {.x = WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
        double ir = WAVY_CIRCLE_DEFAULT_TRANSLATION + WAVY_CIRCLE_AMPLITUDE * sin(WAVY_CIRCLE_FREQUENCY * theta);
        double vr = WAVY_CIRCLE_DEFAULT_TRANSLATION - WAVY_CIRCLE_AMPLITUDE * sin(WAVY_CIRCLE_FREQUENCY * theta);

        vector_t *position_invisible = malloc(sizeof(vector_t));
        *position_invisible = (vector_t){.x = ir * cos(theta), .y = ir * sin(theta)};
        position_invisible->x = vec_add(center, *position_invisible).x;
        position_invisible->y = vec_add(center, *position_invisible).y;
        make_ball(scene, position_invisible, BALL_INVISIBLE_RADIUS, BALL_INVISIBLE_COLOR);
        create_drag(scene, DRAG_GAMMA, scene_get_body(scene, 2 * i));

        vector_t *position_visible = malloc(sizeof(vector_t));
        *position_visible = (vector_t){.x = vr * cos(theta), .y = vr * sin(theta)};
        position_visible->x = vec_add(center, *position_visible).x;
        position_visible->y = vec_add(center, *position_visible).y;
        color = color_rainbow(i);
        make_ball(scene, position_visible, BALL_RADIUS, color);

        create_drag(scene, DRAG_GAMMA, scene_get_body(scene, 2 * i + 1));

        create_spring(scene, SPRING_CONSTANT_K, scene_get_body(scene, 2 * i),
                      scene_get_body(scene, 2 * i + 1));

        theta += 2 * M_PI / (double)NUM_BALLS;
    }
}

void reset_wavy_circle(scene_t *scene, double translation)
{
    double theta = 0;

    for (int i = 0; i < NUM_BALLS; i++)
    {
        vector_t center = {.x = WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
        double ir = translation + WAVY_CIRCLE_AMPLITUDE * sin(WAVY_CIRCLE_FREQUENCY * theta);
        double vr = translation - WAVY_CIRCLE_AMPLITUDE * sin(WAVY_CIRCLE_FREQUENCY * theta);

        vector_t *position_invisible = malloc(sizeof(vector_t));
        *position_invisible = (vector_t){.x = ir * cos(theta), .y = ir * sin(theta)};
        position_invisible->x = vec_add(center, *position_invisible).x;
        position_invisible->y = vec_add(center, *position_invisible).y;
        body_set_centroid(scene_get_body(scene, 2 * i), *position_invisible);

        vector_t *position_visible = malloc(sizeof(vector_t));
        *position_visible = (vector_t){.x = vr * cos(theta), .y = vr * sin(theta)};
        position_visible->x = vec_add(center, *position_visible).x;
        position_visible->y = vec_add(center, *position_visible).y;
        body_set_centroid(scene_get_body(scene, 2 * i + 1), *position_visible);

        theta += 2 * M_PI / (double)NUM_BALLS;
    }
}

void wavy_circle_key_handler(char key, key_event_type_t type, double held_time, void *obj)
{
    scene_t *scene = (scene_t *)obj;

    if (type == KEY_PRESSED && key == UP_ARROW)
    {
        reset_wavy_circle(scene, WAVY_CIRCLE_BIG_TRANSLATION);
    }
    else if (type == KEY_PRESSED && key == DOWN_ARROW)
    {
        reset_wavy_circle(scene, WAVY_CIRCLE_SMALL_TRANSLATION);
    }
    else if ((type == KEY_PRESSED && key == LEFT_ARROW) || (type == KEY_PRESSED && key == RIGHT_ARROW))
    {
        reset_wavy_circle(scene, WAVY_CIRCLE_DEFAULT_TRANSLATION);
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
    make_wavy_circle(scene);
    sdl_on_key(wavy_circle_key_handler);

    while (!sdl_is_done(scene))
    {
        double dt = time_since_last_tick();
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }

    scene_free(scene);
}
