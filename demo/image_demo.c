#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "color.h"
#include "vector.h"
#include "text.h"
#include "polygon.h"
#include "forces.h"
#include "list.h"
#include "image.h"
#include "scene.h"
#include "body.h"
#include "sdl_wrapper.h"

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

void update_image(image_t *image, body_t *body);
void create_wall(scene_t *scene, body_t *ball, double x1, double y1, double x2, double y2);
void build_walls(scene_t *scene, body_t *ball);

void update_image(image_t *image, body_t *body)
{
    vector_t position = body_get_centroid(body);
    position.y = WINDOW_HEIGHT - position.y;
    image_set_position(image, position);
}

void create_wall(scene_t *scene, body_t *ball, double x1, double y1, double x2, double y2)
{
    list_t *shape = polygon_make_rectangle(x1, y1, x2, y2);
    body_t *wall = body_init(shape, INFINITY, (rgb_color_t){0,0,0});
    scene_add_body(scene, wall);
    create_physics_collision(scene, 1, wall, ball);
}

void build_walls(scene_t *scene, body_t *ball)
{
    // left
    create_wall(scene, ball, -1.0 * 5, 0, 0, WINDOW_HEIGHT);
    // right
    create_wall(scene, ball, WINDOW_WIDTH, 0, WINDOW_WIDTH + 5, WINDOW_HEIGHT);
    // top
    create_wall(scene, ball, -1.0 * 5, WINDOW_HEIGHT, WINDOW_WIDTH + 5, WINDOW_HEIGHT + 5);
    // bottom
    create_wall(scene, ball, 0, -1.0 * 5, WINDOW_WIDTH, 0);
}

int main(int argc, char **argv)
{
    sdl_init(VEC_ZERO, (vector_t){.x = 1000, .y = 500});
    scene_t *scene = scene_init();

    vector_t position = {.x = 500, .y = 250};
    vector_t size = {.x = 30, .y = 30};

    list_t *shape = polygon_make_circle(position, 30, 10);
    body_t *body = body_init_with_info_with_image(shape, 10, (rgb_color_t){0,0,0}, NULL, NULL, "images/test_sprite.png", IMG_INIT_PNG, size);
    body_set_velocity(body, (vector_t){100.0, 100.0});
    build_walls(scene, body);
    scene_add_body(scene, body);

    while (!sdl_is_done(NULL))
    {
        double dt = time_since_last_tick();
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }

    sdl_cleanup();
    return EXIT_SUCCESS;
}
