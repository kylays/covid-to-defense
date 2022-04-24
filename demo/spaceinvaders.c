#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "list.h"
#include "body.h"
#include "vector.h"
#include "sdl_wrapper.h"
#include "scene.h"
#include "forces.h"

#define DEMO_WINDOW_WIDTH 1000
#define DEMO_WINDOW_HEIGHT 1500

typedef struct
{
    int body_id;
} body_type_t;

const int PLAYER_ID = 1;
const int INVADER_ID = 2;
const int PLAYER_BULLET_ID = 3;
const int INVADER_BULLET_ID = 4;

// PLAYER CONFIGURATION
const double PLAYER_MASS = 10;
const size_t PLAYER_NUM_POINTS = 20;
const rgb_color_t PLAYER_COLOR = {.r = 0.25, .b = 1, .g = .5};
const double PLAYER_SIZE = 50;
const double PLAYER_XY_RATIO = 0.5;
const double PLAYER_VELOCITY = 300.0;
const double PLAYER_ACCELERATION = 250.0;

// INVADERS CONFIGURATION
const double INVADERS_SPACING = 25;
const double INVADERS_BULLET_DELAY = 1.5;
const double INVADERS_COL_SIZE = 3.0;
const double INVADERS_ROW_SIZE = 7.0;
const double INVADER_RADIUS = (DEMO_WINDOW_WIDTH - (25 * 7)) / 14.0;
const vector_t INVADERS_START_POSITION = {.x = 25 + ((DEMO_WINDOW_WIDTH - (25 * 7)) / 14.0),
                                          .y = DEMO_WINDOW_HEIGHT - 25 - (3 * (DEMO_WINDOW_WIDTH - (25 * 7)) / 14.0)};
const double INVADER_FRAC = 6.0;
const double INVADER_MASS = 100;
const double INVADER_SPEED = 100;
const vector_t INVADER_START_VELOCITY = {.x = 100, .y = 0};
const size_t INVADER_NUM_POINTS = 15;
const rgb_color_t INVADER_COLOR = {.r = 0, .b = 0, .g = 0};
const size_t UPPER_LIMIT = 100;

// BULLET CONFIGURATION
const double BULLET_SIZE = 10;
const double SPACE_BULLET_MASS = 10;
const size_t SPACE_BULLET_NUM_POINTS = 5;
const vector_t PLAYER_BULLET_VELOCITY = {.x = 0, .y = 300};
const vector_t INVADER_BULLET_VELOCITY = {.x = 0, .y = -200};
const rgb_color_t INVADER_BULLET_COLOR = {.r = 0, .b = 0, .g = 0};
const rgb_color_t PLAYER_BULLET_COLOR = {.r = 0.25, .b = 1, .g = .5};

// FUNCTION DECLARATIONS
body_type_t *create_body_type(int type);
int get_body_type(body_t *body);
body_t *setup_player(scene_t *scene);
void contain_player_movement(scene_t *scene);
body_t *one_invader(scene_t *scene, vector_t position);
void setup_invaders(scene_t *scene);
void handle_invader_teleport(scene_t *scene);
void spaceinvaders_key_handler(char key, key_event_type_t type, double held_time, scene_t *scene);
void shoot_bullet(scene_t *scene, vector_t position, rgb_color_t color, int bullet_type, vector_t velocity);
void add_collisions(scene_t *scene);
void delete_extra_bullets(scene_t *scene);
vector_t get_rand_invader_bullet_pos(scene_t *scene);
bool game_lost(scene_t *scene);

// FUNCTIONS
body_type_t *create_body_type(int type)
{
    body_type_t *t = malloc(sizeof(body_type_t));
    t->body_id = type;
    return t;
}

int get_body_type(body_t *body)
{
    assert(body_get_info(body) != NULL);
    if (body_get_info(body) != NULL)
    {
        body_type_t type = (*((body_type_t *)body_get_info(body)));
        return type.body_id;
    }
    // if the body does not have a type
    return -1;
}

body_t *setup_player(scene_t *scene)
{
    list_t *shape = list_init(PLAYER_NUM_POINTS, free);

    // player circle
    for (double i = 0; i < 2 * M_PI; i += 2 * M_PI / PLAYER_NUM_POINTS)
    {
        double x = PLAYER_SIZE * cos(i);
        double y = PLAYER_SIZE * PLAYER_XY_RATIO * sin(i);

        vector_t *point = malloc(sizeof(vector_t));
        point->x = x;
        point->y = y;

        list_add(shape, point);
    }

    body_t *player = body_init_with_info(shape, PLAYER_MASS, PLAYER_COLOR, create_body_type(PLAYER_ID), free);
    body_set_centroid(player, (vector_t){.x = DEMO_WINDOW_WIDTH / 2.0, .y = PLAYER_SIZE});
    scene_add_body(scene, player);
    return player;
}

void contain_player_movement(scene_t *scene)
{
    // get player body
    body_t *player = scene_get_body(scene, 0);
    for (size_t i = 0; i < scene_bodies(scene); i++)
    {
        if (get_body_type(scene_get_body(scene, i)) == PLAYER_ID) {
            player = scene_get_body(scene, i);
        }
    }
    vector_t center = body_get_centroid(player);

    // handling wrap around in the x direction
    if (center.x + PLAYER_SIZE > DEMO_WINDOW_WIDTH)
    {
        center.x = DEMO_WINDOW_WIDTH - PLAYER_SIZE;
    }
    else if (center.x - PLAYER_SIZE < 0)
    {
        center.x = PLAYER_SIZE;
    }
    // handling wrap around in the y direction
    else if (center.y + PLAYER_XY_RATIO * PLAYER_SIZE > DEMO_WINDOW_HEIGHT)
    {
        center.y = DEMO_WINDOW_HEIGHT - PLAYER_XY_RATIO * PLAYER_SIZE;
    }
    else if (center.y - PLAYER_XY_RATIO * PLAYER_SIZE < 0)
    {
        center.y = PLAYER_XY_RATIO * PLAYER_SIZE;
    }
    else
    {
        return;
    }
    body_set_centroid(player, center);
}

body_t *one_invader(scene_t *scene, vector_t position)
{
    // makes one invader body
    list_t *shape = list_init(INVADER_NUM_POINTS * 2, free);
    vector_t *center = malloc(sizeof(vector_t));
    center->x = position.x;
    center->y = position.y;

    // center point
    list_add(shape, center);

    // body partial circle
    double arc = (1.0 - 1.0 / INVADER_FRAC) * M_PI - M_PI / INVADER_FRAC;
    double arc_start = M_PI / INVADER_FRAC;
    for (double i = arc_start; i < arc_start + arc; i += arc / INVADER_NUM_POINTS)
    {
        double x = INVADER_RADIUS * cos(i);
        double y = INVADER_RADIUS * sin(i);

        vector_t *point = malloc(sizeof(vector_t));
        point->x = center->x + x;
        point->y = center->y + y;

        list_add(shape, point);
    }

    body_t *invader = body_init_with_info(shape, INVADER_MASS, INVADER_COLOR, create_body_type(INVADER_ID), free);
    body_set_centroid(invader, (vector_t){.x = position.x, .y = position.y});
    body_set_velocity(invader, INVADER_START_VELOCITY);
    scene_add_body(scene, invader);
    return invader;
}

void setup_invaders(scene_t *scene)
{
    // makes all the starting invader bodies by making the position vectors and calling one_invader()
    // make each column of invaders at a time (need to be able to group them together to teleport)

    double x = INVADERS_START_POSITION.x;
    double y = INVADERS_START_POSITION.y - (INVADERS_COL_SIZE * 0.5);
    double dx = INVADER_RADIUS * 2 + INVADERS_SPACING;
    double dy = INVADER_RADIUS + INVADERS_SPACING;

    for (size_t i = 1; i <= (size_t)(INVADERS_ROW_SIZE * INVADERS_COL_SIZE); i++)
    {
        vector_t position = {.x = x, .y = y};
        one_invader(scene, position);

        if (i % (int)INVADERS_COL_SIZE == 0)
        {
            x += dx;
            y = INVADERS_START_POSITION.y;
        }
        else
        {
            y += dy;
        }
    }
}

void handle_invader_teleport(scene_t *scene)
{
    for (size_t i = 0; i < scene_bodies(scene); i++)
    {

        body_t *body = scene_get_body(scene, i);
        vector_t centroid = body_get_centroid(body);

        if (get_body_type(body) == INVADER_ID)
        {
            if ((body_get_velocity(body).x > 0 && centroid.x + INVADER_RADIUS >= DEMO_WINDOW_WIDTH) ||
                (body_get_velocity(body).x < 0 && centroid.x - INVADER_RADIUS <= 0))
            {
                centroid.y -= (INVADER_RADIUS * INVADERS_COL_SIZE + INVADERS_SPACING * INVADERS_COL_SIZE);
                body_set_centroid(body, centroid);
                vector_t velocity = body_get_velocity(body);
                body_set_velocity(body, vec_negate(velocity));
            }
        }
    }
}

void spaceinvaders_key_handler(char key, key_event_type_t type, double held_time, scene_t *scene)
{
    body_t *player = scene_get_body(scene, 0);
    vector_t centroid = body_get_centroid(player);
    double new_speed = vec_magnitude(body_get_velocity(player)) + PLAYER_ACCELERATION * held_time;

    if (type == KEY_PRESSED)
    {
        switch (key)
        {
        case UP_ARROW:
            body_set_velocity(player, (vector_t){.x = 0, .y = new_speed});
            body_set_centroid(player, centroid);
            break;
        case DOWN_ARROW:
            body_set_velocity(player, (vector_t){.x = 0, .y = -new_speed});
            body_set_centroid(player, centroid);
            break;
        case LEFT_ARROW:
            body_set_velocity(player, (vector_t){.x = -new_speed, .y = 0});
            body_set_centroid(player, centroid);
            break;
        case RIGHT_ARROW:
            body_set_velocity(player, (vector_t){.x = new_speed, .y = 0});
            body_set_centroid(player, centroid);
            break;
        case SPACE:
            shoot_bullet(scene, centroid, PLAYER_BULLET_COLOR, PLAYER_BULLET_ID, PLAYER_BULLET_VELOCITY);
            break;
        case ESCAPE:
            printf("ESC pressed GAME OVER\n");
            exit(1);
        }
    }
    else if (type == KEY_RELEASED)
    {
        body_set_velocity(player, VEC_ZERO);
    }
}

void shoot_bullet(scene_t *scene, vector_t position, rgb_color_t color, int bullet_type, vector_t velocity)
{
    list_t *shape = list_init(SPACE_BULLET_NUM_POINTS, free);

    for (double i = 0; i < 2 * M_PI; i += 2 * M_PI / SPACE_BULLET_NUM_POINTS)
    {
        double x = BULLET_SIZE * cos(i);
        double y = BULLET_SIZE * sin(i);

        vector_t *point = malloc(sizeof(vector_t));
        point->x = position.x + x;
        point->y = position.y + y;

        list_add(shape, point);
    }

    body_t *bullet = body_init_with_info(shape, SPACE_BULLET_MASS, color, create_body_type(bullet_type), free);
    body_set_velocity(bullet, velocity);
    scene_add_body(scene, bullet);

    for (size_t j = 0; j < scene_bodies(scene); j++)
    {
        body_t *other_body = scene_get_body(scene, j);
        if (get_body_type(other_body) == INVADER_ID && bullet_type == PLAYER_BULLET_ID)
        {
            create_destructive_collision(scene, bullet, other_body);
        }
        else if (get_body_type(other_body) == PLAYER_ID && bullet_type == INVADER_BULLET_ID)
        {
            create_destructive_collision(scene, bullet, other_body);
        }
    }
}

void add_collisions(scene_t *scene)
{
    for (size_t i = 0; i < scene_bodies(scene); i++)
    {
        // add collision force between player and invader bodies
        body_t *current_body = scene_get_body(scene, i);
        if (get_body_type(current_body) == PLAYER_ID)
        {
            for (size_t j = 0; j < scene_bodies(scene); j++)
            {
                body_t *other_body = scene_get_body(scene, j);
                if (get_body_type(other_body) == INVADER_ID)
                {
                    create_destructive_collision(scene, current_body, other_body);
                }
            }
        }
    }
}

void delete_extra_bullets(scene_t *scene)
{
    // delete the bullets when they go off the screen
    for (size_t j = 0; j < scene_bodies(scene); j++)
    {
        body_t *body = scene_get_body(scene, j);
        if ((get_body_type(body) == INVADER_BULLET_ID ||
             get_body_type(body) == PLAYER_BULLET_ID) &&
            (body_get_centroid(body).y > DEMO_WINDOW_HEIGHT + BULLET_SIZE ||
             body_get_centroid(body).y < 0 - BULLET_SIZE))
        {
            scene_remove_body(scene, j);
        }
    }
}

vector_t get_rand_invader_bullet_pos(scene_t *scene)
{
    // get a list of invaders
    list_t *invaders = list_init(10, free);
    for (int i = 0; i < scene_bodies(scene); i++)
    {
        body_t *body = scene_get_body(scene, i);
        if (get_body_type(body) == INVADER_ID)
        {
            list_add(invaders, body);
        }
    }

    // get random invader from the invaders list
    if (list_size(invaders) > 0)
    {
        size_t random_index = rand() % list_size(invaders);
        return body_get_centroid(list_get(invaders, random_index));
    }
    else
    {
        return (vector_t){.x = 0, .y = 0};
    }
}

bool game_lost(scene_t *scene)
{
    for (int i = 0; i < scene_bodies(scene); i++)
    {
        body_t *body = scene_get_body(scene, i);
        if (get_body_type(body) == INVADER_ID && body_get_centroid(body).y < 0)
        {
            return true;
        }
    }
    if (get_body_type(scene_get_body(scene, 0)) != PLAYER_ID)
    {
        return true;
    }
    return false;
}

int main()
{
    srand(42);

    sdl_init(VEC_ZERO, (vector_t){.x = DEMO_WINDOW_WIDTH, .y = DEMO_WINDOW_HEIGHT});
    sdl_on_key((key_handler_t) spaceinvaders_key_handler);

    scene_t *scene = scene_init();
    setup_player(scene);
    setup_invaders(scene);
    add_collisions(scene);
    double counter = INVADERS_BULLET_DELAY;

    while (!sdl_is_done(scene))
    {
        contain_player_movement(scene);
        if (counter >= INVADERS_BULLET_DELAY)
        {
            // shoot invader bullets according to a time delay from a random invader
            vector_t rand_pos = get_rand_invader_bullet_pos(scene);
            shoot_bullet(scene, rand_pos, INVADER_BULLET_COLOR, INVADER_BULLET_ID, INVADER_BULLET_VELOCITY);
            counter = 0.0;
        }
        if (game_lost(scene))
        {
            printf("YOU LET THE INVADERS IN! GAME OVER, YOU LOSE :( \n");
            break;
        }
        else if (scene_bodies(scene) == 1 || ((scene_bodies(scene) > 1 &&
                    get_body_type(scene_get_body(scene, 1)) != INVADER_ID)))
        {
            printf("YOU STOPPED THE INVADERS! YOU WIN :) \n");
            break;
        }

        // UPDATE
        double dt = time_since_last_tick();
        handle_invader_teleport(scene);

        // DRAW
        scene_tick(scene, dt);
        sdl_render_scene(scene);

        delete_extra_bullets(scene);
        counter += dt;
    }

    // CLEANUP
    scene_free(scene);
    return 0;
}
