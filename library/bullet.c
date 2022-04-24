#include "bullet.h"
#include "scene.h"
#include "forces.h"
#include "tower.h"

const double BULLET_RADIUS = 5.0;
const size_t BULLET_NUM_POINTS = 5;
const double BULLET_MASS = 10.0;
const vector_t BOMB_IMG_SIZE = {.x = 50, .y = 50};

// private
void bullet_delete_out_of_window(body_t *bullet, body_t *wall, vector_t axis, scene_t *scene)
{
    body_remove(bullet);
}

body_t *create_basic_bullet(scene_t *scene, vector_t position, rgb_color_t color, bullet_t bullet_info)
{
    bullet_t *info = malloc(sizeof(bullet_t));
    *info = bullet_info;
    list_t *shape = polygon_make_circle(position, BULLET_RADIUS, BULLET_NUM_POINTS);
    body_t *bullet = body_init_with_secondary_info(shape, BULLET_MASS, color, info, free, BULLET_TYPE);
    scene_add_body(scene, bullet);

    // add collision force with playing window borders to detect when viruses complete the path
    for (size_t i = 0; i < scene_bodies(scene); i++)
    {
        body_t *curr_body = scene_get_body(scene, i);
        // if bullet hits a wall
        if (get_global_type(curr_body) == WALL_TYPE)
        {
            create_collision(scene, bullet, curr_body, (collision_handler_t)bullet_delete_out_of_window, scene, NULL);
        }
    }

    return bullet;
}

body_t *create_explosion_effect_bullet(scene_t *scene, vector_t position)
{

    rgb_color_t color = BLACK;

    bullet_t *info = malloc(sizeof(bullet_t));
    *info = (bullet_t){0, 0, 0, 0, 3}; // 3 alive ticks
    list_t *shape = polygon_make_circle(position, BULLET_RADIUS, BULLET_NUM_POINTS);
    body_t *bullet = body_init_with_secondary_info(shape, BULLET_MASS, color, info, free, BULLET_TYPE);

    image_t *image = image_init("images/bomb.png", position, BOMB_IMG_SIZE, IMG_INIT_PNG);
    body_set_image(bullet, image);

    scene_add_body(scene, bullet);

    return bullet;
}

void bullet_tick(body_t *bullet)
{

    bullet_t *info = get_global_secondary_info(bullet);
    if (info->alive_ticks <= 0)
    {
        body_remove(bullet);
    }
    else
    {
        info->alive_ticks -= 1;
    }
}

double bullet_get_radius()
{
    return BULLET_RADIUS;
}

bullet_t bullet_make_struct(double speed, int damage, double glue_slowdown,
                            int num_tack_directions, double alive_ticks)
{
    return (bullet_t){.speed = speed, .damage = damage, .glue_slowdown = glue_slowdown, 
                        .num_tack_directions = num_tack_directions, .alive_ticks = alive_ticks};
}

double bullet_get_mass()
{
    return BULLET_MASS;
}

size_t bullet_get_num_points()
{
    return BULLET_NUM_POINTS;
}
