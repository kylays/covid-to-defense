#ifndef BULLET_H
#define BULLET_H

#include "body.h"
#include "forces.h"
#include "vector.h"
#include "scene.h"
#include "global_body_info.h"

/**
 *
 * @brief a features_aux; stores info about the bullets a tower shoots
 * double speed: bullet speed
 * int damage: amount of health points a bullet takes away when hitting a COVID particle
 * double glue_slowdown: if the tower is a glue tower, this is the slowdown factor
 * bool pursuit: stores T/F for if the bullet is a pursuit bullet (airplane tower) or not
 */
typedef struct bullet_t
{
    double speed;
    int damage;
    double glue_slowdown;
    int num_tack_directions;
    double alive_ticks;
} bullet_t;

/**
 * @brief Create a basic bullet object
 *
 * @param scene scene where bullet will be displayed
 * @param position initial position of bullet
 * @param color color of bullet
 * @return pointer to newly allocated bullet
 */
body_t *create_basic_bullet(scene_t *scene, vector_t position, rgb_color_t color, bullet_t info);

body_t *create_explosion_effect_bullet(scene_t *scene, vector_t position);
/**
 * @brief deletes bullets after their alive_ticks
 * 
 * @param bullet to be updated
 */
void bullet_tick(body_t *bullet);

/**
 * @brief gets the bullet radius
 *
 * @return bullet size
 */
double bullet_get_radius();

/**
 * @brief gets the bullet num points in the shape list
 *
 * @return num points of bullet shape
 */
size_t bullet_get_num_points();

/**
 * @brief gets the bullet mass
 *
 * @return bullet mass
 */
double bullet_get_mass();

/**
 * @brief returns a bullet_t struct with the given info inside
 * 
 * @param speed 
 * @param damage 
 * @param glue_slowdown 
 * @param num_tack_directions 
 * @return bullet_t 
 */
bullet_t bullet_make_struct(double speed, int damage, double glue_slowdown, int num_tack_directions, double alive_ticks);

#endif