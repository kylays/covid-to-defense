#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "list.h"
#include "body.h"
#include "vector.h"
#include "sdl_wrapper.h"
#include "scene.h"

// PACMAN CONFIGURATION
const double PACMAN_SIZE = 40;
const double PACMAN_MOUTH_FRAC = 6.0;
const double PACMAN_MASS = 100;
const double PACMAN_ACCELERATION = 400;
const double PACMAN_BASE_SPEED = 300;
const size_t PACMAN_NUM_POINTS = 20;
const rgb_color_t PACMAN_COLOR = {.r = 1, .b = 1, .g = 0};

// PELLET CONFIGURATION
const double PELLET_SIZE = 10;
const double SPAWN_PELLET_INTERVAL = 10;
const size_t NUM_STARTING_PELLETS = 20;
const double PELLET_MASS = 20;
const size_t PELLET_NUM_POINTS = 20;
const rgb_color_t PELLET_COLOR = {.r = 1, .b = 1, .g = 0};

body_t *setup_pacman(scene_t *scene);
void handle_pacman_wraparound(body_t *pacman);
void pacman_key_handler(char key, key_event_type_t type, double held_time, body_t *pacman);
void spawn_pellet(scene_t *scene);
void handle_collisions(body_t *pacman, scene_t *scene);

body_t *setup_pacman(scene_t *scene)
{
    list_t *shape = list_init(PACMAN_NUM_POINTS * 2, free);

    vector_t *center = malloc(sizeof(vector_t));
    center->x = 0;
    center->y = 0;

    // center point
    list_add(shape, center);

    // pacman circle
    double arc = (1 - 1 / PACMAN_MOUTH_FRAC) * 2 * M_PI;
    double arc_start = M_PI / PACMAN_MOUTH_FRAC;
    for (double i = arc_start; i < arc_start + arc; i += arc / PACMAN_NUM_POINTS)
    {
        double x = PACMAN_SIZE * cos(i);
        double y = PACMAN_SIZE * sin(i);

        vector_t *point = malloc(sizeof(vector_t));
        point->x = center->x + x;
        point->y = center->y + y;

        list_add(shape, point);
    }

    body_t *pacman = body_init(shape, PACMAN_MASS, PACMAN_COLOR);
    body_set_centroid(pacman, (vector_t){.x = WINDOW_WIDTH / 2, .y = WINDOW_HEIGHT / 2});
    scene_add_body(scene, pacman);
    return pacman;
}

void handle_pacman_wraparound(body_t *pacman)
{
    vector_t center = body_get_centroid(pacman);

    // handling wrap around in the x direction
    if (center.x > WINDOW_WIDTH)
    {
        center.x = 0;
    }
    else if (center.x < 0)
    {
        center.x = WINDOW_WIDTH;
    }
    // handling wrap around in the y direction
    else if (center.y > WINDOW_HEIGHT)
    {
        center.y = 0;
    }
    else if (center.y < 0)
    {
        center.y = WINDOW_HEIGHT;
    }
    else
    {
        return;
    }
    body_set_centroid(pacman, center);
}

void pacman_key_handler(char key, key_event_type_t type, double held_time, body_t *pacman)
{
    double new_speed = PACMAN_BASE_SPEED + PACMAN_ACCELERATION * held_time;

    if (type == KEY_PRESSED)
    {
        switch (key)
        {
        case UP_ARROW:
            printf("UP pressed\n");
            body_set_velocity(pacman, (vector_t){.x = 0, .y = new_speed});
            body_set_rotation(pacman, M_PI / 2);
            break;
        case DOWN_ARROW:
            printf("DOWN pressed\n");
            body_set_velocity(pacman, (vector_t){.x = 0, .y = -new_speed});
            body_set_rotation(pacman, 3 * M_PI / 2);
            break;
        case LEFT_ARROW:
            printf("LEFT pressed\n");
            body_set_velocity(pacman, (vector_t){.x = -new_speed, .y = 0});
            body_set_rotation(pacman, M_PI);
            break;
        case RIGHT_ARROW:
            printf("RIGHT pressed\n");
            body_set_velocity(pacman, (vector_t){.x = new_speed, .y = 0});
            body_set_rotation(pacman, 0);
            break;
        case ESCAPE:
            printf("ESC pressed\n");
            exit(1);
        }
    }
    else
    {
        vector_t velocity = body_get_velocity(pacman);
        if (velocity.y == 0)
        {
            if (velocity.x > PACMAN_BASE_SPEED)
            {
                body_set_velocity(pacman, (vector_t){.x = PACMAN_BASE_SPEED, .y = 0});
            }
            else if (velocity.x < PACMAN_BASE_SPEED)
            {
                body_set_velocity(pacman, (vector_t){.x = -PACMAN_BASE_SPEED, .y = 0});
            }
        }
        else if (velocity.x == 0)
        {
            if (velocity.y > PACMAN_BASE_SPEED)
            {
                body_set_velocity(pacman, (vector_t){.x = 0, .y = PACMAN_BASE_SPEED});
            }
            else if (velocity.y < PACMAN_BASE_SPEED)
            {
                body_set_velocity(pacman, (vector_t){.x = 0, .y = -PACMAN_BASE_SPEED});
            }
        }
    }
}

void spawn_pellet(scene_t *scene)
{
    vector_t *position = malloc(sizeof(vector_t));
    position->x = (double)rand() / (double)RAND_MAX * (double)WINDOW_WIDTH;
    position->y = (double)rand() / (double)RAND_MAX * (double)WINDOW_HEIGHT;

    // create the pellet body
    list_t *shape = list_init(PELLET_NUM_POINTS, free);

    for (double i = 0; i < 2 * M_PI; i += 2 * M_PI / PELLET_NUM_POINTS)
    {
        double x = PELLET_SIZE * cos(i);
        double y = PELLET_SIZE * sin(i);

        vector_t *point = malloc(sizeof(vector_t));
        point->x = position->x + x;
        point->y = position->y + y;

        list_add(shape, point);
    }

    body_t *pellet = body_init(shape, PELLET_MASS, PELLET_COLOR);
    scene_add_body(scene, pellet);
}

void handle_collisions(body_t *pacman, scene_t *scene)
{
    size_t index = 1;

    while (index < scene_bodies(scene))
    {

        body_t *pellet = scene_get_body(scene, index);
        vector_t pellet_center = body_get_centroid(pellet);
        vector_t pacman_center = body_get_centroid(pacman);
        double distance = vec_magnitude(vec_subtract(pellet_center, pacman_center));

        if (distance < PACMAN_SIZE - PELLET_SIZE)
        {
            scene_remove_body(scene, index);
        }
        index++;
    }
}

int main()
{
    double pellet_counter = 0;

    sdl_init(VEC_ZERO, (vector_t){.x = WINDOW_WIDTH, .y = WINDOW_HEIGHT});
    scene_t *scene = scene_init();
    sdl_on_key((key_handler_t) pacman_key_handler);
    body_t *pacman = setup_pacman(scene);

    for (int i = 0; i < NUM_STARTING_PELLETS; i++)
    {
        spawn_pellet(scene);
    }

    while (!sdl_is_done(pacman))
    {
        double dt = time_since_last_tick();

        // DRAW
        scene_tick(scene, dt); // updating graphics
        sdl_render_scene(scene);

        // UPDATE
        handle_pacman_wraparound(pacman);
        if (pellet_counter > SPAWN_PELLET_INTERVAL)
        {
            spawn_pellet(scene);
            pellet_counter = 0;
        }

        pellet_counter += dt;
        handle_collisions(pacman, scene);
    }

    // CLEANUP
    scene_free(scene);

    return 0;
}