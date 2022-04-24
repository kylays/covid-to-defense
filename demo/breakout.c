#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

// SOUND
#include <SDL2/SDL.h>

#include "list.h"
#include "body.h"
#include "vector.h"
#include "sdl_wrapper.h"
#include "scene.h"
#include "forces.h"
#include "collision.h"

// -----------------------------------------------------------------------------

#define DEMO_WINDOW_WIDTH 1000
#define DEMO_WINDOW_HEIGHT 1250

const int PLAYER_ID = 1;
const int BRICK_ID = 2;
const int BALL_ID = 3;
const int WALL_ID = 4;

const int BREAKOUT_RECTANGLE_NUM_SIDES = 4;

// WALL CONFIGURATION
const double WALL_MASS = INFINITY;
const double WALL_WIDTH = 1.0;
const rgb_color_t WALL_COLOR = {.r = 1.0, .b = 1.0, .g = 1.0};
const size_t NUM_WALLS = 3;

// PLAYER (PADDLE) CONFIGURATION
const double PLAYER_MASS = INFINITY;
const double PLAYER_VELOCITY = 3000.0;
const double PLAYER_ACCELERATION = 250.0;
const int PLAYER_INITIAL_HEALTH = 5;
const double PLAYER_WIDTH = 150.0;
const double PLAYER_HEIGHT = 25.0;
const double PLAYER_INITIAL_Y = 30.0;
const double PLAYER_ELASTICITY = 1.0;
const rgb_color_t PLAYER_COLOR = {.r = 0.25, .b = 1.0, .g = .5};

// BALL CONFIGURATION
const double BALL_RADIUS = 15.0;
const double BALL_MASS = 10.0;
const double BALL_ELASTICITY = 1.0;
const size_t BALL_NUM_POINTS = 10;
const vector_t BALL_VELOCITY = {.x = 5000, .y = 5000};
const rgb_color_t BALL_COLOR = {.r = 0.25, .b = 1.0, .g = .5};
const size_t NUM_BALLS = 1;

// BRICK CONFIGURATION
const double BRICK_WIDTH = 100.0;
const double BRICK_HEIGHT = 50.0;
const double BRICK_INITIAL_Y = DEMO_WINDOW_HEIGHT - 50.0;
const double BRICK_INITIAL_X = 0.0;
const double BRICK_X_SPACING = 10.0;
const double BRICK_Y_SPACING = 25.0;
const size_t BRICK_LAYERS = 3;
const size_t BRICKS_PER_LAYER = 9;
const double BRICK_MASS = INFINITY;
const int BRICK_INITIAL_HEALTH = 3;
const size_t COLOR_CHANGE = 3;

typedef struct
{
    int body_id;
    int health;
} body_type_t;

// -----------------------------------------------------------------------------

// FUNCTION DECLARATIONS
// BODY ID
body_type_t *create_body_type(int type, int health);
int get_body_type(body_t *body);

// MAKE BODIES
list_t *create_rectangle(double x1, double y1, double x2, double y2);
void build_brick(scene_t *scene, vector_t pos, rgb_color_t color, body_t *ball);
void build_layer(scene_t *scene, double y, body_t *ball);
body_t *setup_player(scene_t *scene, body_t *player);
void create_wall(scene_t *scene, body_t *ball, double x1, double y1, double x2, double y2);
void build_walls(scene_t *scene, body_t *ball);
body_t *build_ball(scene_t *scene);

// GAMEPLAY
void contain_player_movement(scene_t *scene);
void breakout_collision(body_t *body1, body_t *body2, vector_t axis, void *aux);
body_t *get_ball(scene_t *scene);
bool game_lost(scene_t *scene);
void start_game(scene_t *scene);
void breakout_key_handler(char key, key_event_type_t type, double held_time, scene_t *scene);

// SOUND
void play_music();
void my_audio_callback(void *userdata, Uint8 *stream, int len);

// -----------------------------------------------------------------------------

// FUNCTIONS
body_type_t *create_body_type(int type, int health)
{
    body_type_t *t = malloc(sizeof(body_type_t));
    t->health = health;
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

void breakout_collision(body_t *ball, body_t *brick, vector_t axis, void *aux)
{
    body_type_t *info = body_get_info(brick);
    info->health--;

    if (info->health <= 0)
    {
        body_remove(brick);
    }
    else
    {
        body_set_color(brick, color_tint(body_get_color(brick)));
    }
}

list_t *create_rectangle(double x1, double y1, double x2, double y2)
{
    // (x1, y1) lower left corner
    // (x2, y2) upper right corner

    list_t *shape = list_init(BREAKOUT_RECTANGLE_NUM_SIDES, free);

    list_add(shape, vec_init(x1, y1));
    list_add(shape, vec_init(x2, y1));
    list_add(shape, vec_init(x2, y2));
    list_add(shape, vec_init(x1, y2));

    return shape;
}

void create_wall(scene_t *scene, body_t *ball, double x1, double y1, double x2, double y2)
{
    list_t *shape = create_rectangle(x1, y1, x2, y2);
    body_t *wall = body_init_with_info(shape, WALL_MASS, WALL_COLOR, create_body_type(WALL_ID, 0), free);
    scene_add_body(scene, wall);
    create_physics_collision(scene, BALL_ELASTICITY, wall, ball);
}

void build_walls(scene_t *scene, body_t *ball)
{
    // left
    create_wall(scene, ball, -1.0 * WALL_WIDTH, 0, 0, DEMO_WINDOW_HEIGHT);
    // right
    create_wall(scene, ball, DEMO_WINDOW_WIDTH, 0, DEMO_WINDOW_WIDTH + WALL_WIDTH, DEMO_WINDOW_HEIGHT);
    // top
    create_wall(scene, ball, -1.0 * WALL_WIDTH, DEMO_WINDOW_HEIGHT, DEMO_WINDOW_WIDTH + WALL_WIDTH, DEMO_WINDOW_HEIGHT + WALL_WIDTH);
}

// since all body elements (except for the bullets) are rectangles
void build_brick(scene_t *scene, vector_t pos, rgb_color_t color, body_t *ball)
{
    list_t *shape = create_rectangle(0, 0, BRICK_WIDTH, BRICK_HEIGHT);

    body_t *brick = body_init_with_info(shape, BRICK_MASS, color, create_body_type(BRICK_ID, BRICK_INITIAL_HEALTH), free);

    // Create collision
    create_collision(scene, ball, brick, (collision_handler_t)breakout_collision, NULL, NULL);
    create_physics_collision(scene, BALL_ELASTICITY, ball, brick);

    body_set_centroid(brick, pos);

    scene_add_body(scene, brick);
}

void build_layer(scene_t *scene, double y, body_t *ball)
{
    size_t color_i = 0;
    double x = BRICK_X_SPACING + BRICK_WIDTH / 2.0;
    for (size_t i = BRICKS_PER_LAYER; i > 0; i--)
    {
        build_brick(scene, (vector_t){x, y}, color_rainbow(color_i), ball);
        x += BRICK_WIDTH + BRICK_X_SPACING;
        color_i += COLOR_CHANGE;
    }
}

body_t *setup_player(scene_t *scene, body_t *ball)
{
    list_t *shape = create_rectangle(0, 0, PLAYER_WIDTH, PLAYER_HEIGHT);

    body_t *player = body_init_with_info(shape, PLAYER_MASS, PLAYER_COLOR, create_body_type(PLAYER_ID, PLAYER_INITIAL_HEALTH), free);
    body_set_centroid(player, (vector_t){.x = DEMO_WINDOW_WIDTH / 2.0, .y = PLAYER_INITIAL_Y});

    scene_add_body(scene, player);
    create_physics_collision(scene, PLAYER_ELASTICITY, ball, player);
    return player;
}

void contain_player_movement(scene_t *scene)
{
    // get player body
    body_t *player = scene_get_body(scene, 1);

    body_set_centroid(player, (vector_t) {body_get_centroid(get_ball(scene)).x, body_get_centroid(player).y});
    return;

    for (size_t i = 0; i < scene_bodies(scene); i++)
    {
        if (get_body_type(scene_get_body(scene, i)) == PLAYER_ID)
        {
            player = scene_get_body(scene, i);
        }
    }
    vector_t center = body_get_centroid(player);

    // handling wrap around in the x direction
    if (center.x + (0.5 * PLAYER_WIDTH) > DEMO_WINDOW_WIDTH)
    {
        center.x = DEMO_WINDOW_WIDTH - (0.5 * PLAYER_WIDTH);
    }
    else if (center.x - (0.5 * PLAYER_WIDTH) < 0)
    {
        center.x = 0.5 * PLAYER_WIDTH;
    }
    else
    {
        return;
    }
    body_set_centroid(player, center);
}

void breakout_key_handler(char key, key_event_type_t type, double held_time, scene_t *scene)
{
    body_t *player = scene_get_body(scene, 1);

    if (type == KEY_PRESSED)
    {
        switch (key)
        {
        case LEFT_ARROW:
            body_set_velocity(player, (vector_t){.x = -PLAYER_VELOCITY, .y = 0});
            break;
        case RIGHT_ARROW:
            body_set_velocity(player, (vector_t){.x = PLAYER_VELOCITY, .y = 0});
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

body_t *build_ball(scene_t *scene)
{
    // make ball circle

    list_t *circle = list_init(BALL_NUM_POINTS, free);
    double theta = 2 * M_PI / BALL_NUM_POINTS;
    vector_t v = {.x = BALL_RADIUS, .y = 0.0};
    for (size_t i = 0; i < BALL_NUM_POINTS; i++)
    {
        vector_t *moved_v = malloc(sizeof(*moved_v));
        *moved_v = v;
        list_add(circle, moved_v);
        v = vec_rotate(v, theta);
    }

    body_t *ball = body_init_with_info(circle, BALL_MASS, BALL_COLOR, create_body_type(BALL_ID, 0), free);
    body_set_centroid(ball, (vector_t){0.5 * DEMO_WINDOW_WIDTH, 0.5 * DEMO_WINDOW_HEIGHT});
    body_set_velocity(ball, BALL_VELOCITY);
    scene_add_body(scene, ball);
    return ball;
}

// retrieves the body that is the ball from the scene
body_t *get_ball(scene_t *scene)
{
    for (size_t i = 0; i < scene_bodies(scene); i++)
    {
        body_t *body = scene_get_body(scene, i);
        if (get_body_type(body) == BALL_ID)
        {
            return body;
        }
    }
    return NULL;
}

// game is lost if ball touches the "ground"
bool game_lost(scene_t *scene)
{
    body_t *ball = get_ball(scene);
    assert(ball != NULL);
    return body_get_centroid(ball).y <= 0;
}

void start_game(scene_t *scene)
{
    body_t *ball = build_ball(scene);

    setup_player(scene, ball);

    for (size_t i = 0; i < BRICK_LAYERS; i++)
    {
        build_layer(scene, BRICK_INITIAL_Y - (i * BRICK_Y_SPACING + i * BRICK_HEIGHT), ball);
    }

    build_walls(scene, ball);
}

// Music Code Source: https://gist.github.com/armornick/3447121
static Uint8 *audio_pos;
static Uint32 audio_len;

void play_music()
{
    static Uint32 wav_length;      // length of our sample
    static Uint8 *wav_buffer;      // buffer containing our audio file
    static SDL_AudioSpec wav_spec; // the specs of our piece of music

    /* Load the WAV */
    // the specs, length and buffer of our wav are filled
    assert(SDL_LoadWAV("sounds/sound2.wav", &wav_spec, &wav_buffer, &wav_length) != NULL);

    // set the callback function
    wav_spec.callback = my_audio_callback;
    wav_spec.userdata = NULL;
    // set our global static variables
    audio_pos = wav_buffer; // copy sound buffer
    audio_len = wav_length; // copy file length

    /* Open the audio device */
    if (SDL_OpenAudio(&wav_spec, NULL) < 0)
    {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        exit(-1);
    }

    /* Start playing */
    SDL_PauseAudio(0);
}

void my_audio_callback(void *userdata, Uint8 *stream, int len)
{
    if (audio_len == 0)
        return;

    len = (len > audio_len ? audio_len : len);
    SDL_memcpy(stream, audio_pos, len);                      // simply copy from one buffer into the other
    SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME); // mix from one buffer into another

    audio_pos += len;
    audio_len -= len;
}

int main()
{
    srand(42);

    sdl_init(VEC_ZERO, (vector_t){.x = DEMO_WINDOW_WIDTH, .y = DEMO_WINDOW_HEIGHT});
    sdl_on_key((key_handler_t)breakout_key_handler);

    play_music();

    scene_t *scene = scene_init();
    start_game(scene);

    while (!sdl_is_done(scene))
    {
        contain_player_movement(scene);

        // UPDATE
        double dt = time_since_last_tick();

        if (game_lost(scene))
        {
            // deleting all bodies from the scene
            for (int i = 0; i < scene_bodies(scene); i++)
            {
                scene_remove_body(scene, i);
            }
            start_game(scene);
        }
        else if (scene_bodies(scene) <= 1 + NUM_WALLS + NUM_BALLS)
        {
            printf("YOU WIN :) \n");
            break;
        }

        // DRAW
        scene_tick(scene, dt);
        sdl_render_scene(scene);


    }

    // CLEANUP
    SDL_CloseAudio();
    scene_free(scene);
    return 0;
}
