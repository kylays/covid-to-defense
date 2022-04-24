#ifndef __STAR_H__
#define __STAR_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>

#include "list.h"
#include "vector.h"
#include "polygon.h"
#include "sdl_wrapper.h"
#include "color.h"

/**
 * A pointed star defined by the following fields:
 * -- vertices: a list of the inner and outer vertices that make up the star
 * -- outer_radius: distance from center to an outer vertex
 * -- inner_radius: default outer_radius / 2.5
 * -- num_points: number of outer points of the star
 * -- rot_angle: rotation angle that the star rotates through on every update()
 * -- center: vector representing the center of the star in the draw window
 * -- velocity: vector representing the velocity of the star in the draw window
 * -- color: the color of the star
 */
typedef struct star
{
    list_t *vertices;
    double outer_radius;
    double inner_radius;
    size_t num_points;
    double rot_angle;
    vector_t center;
    vector_t velocity;
    rgb_color_t color;
} star_t;

/**
 * @brief Initialize a star_t.
 *
 * @param radius: distance from center to an outer vertex
 * @param points: number of outer points of the star
 * @param angle: rotation angle that the star rotates through on every update()
 * @param center: vector representing the center of the star in the draw window
 * @param velocity: vector representing the velocity of the star in the draw window
 * @param color: the color of the star
 * @return star_t pointer
 */
star_t *star_init(vector_t center, vector_t velocity, double radius, size_t points, double angle, rgb_color_t color);

/**
 * @brief Initialize a star_t with default values as defined in star.c file and given
 * num_points. Also sets a rainbow color based on the number of points.
 *
 * @param num_points: number of outer points of the star
 * @return star_t pointer
 */
star_t *star_init_with_n(size_t num_points);

/**
 * @brief Frees the memory allocated for the star and the vertices list_t.
 *
 * @param star The star to free.
 */
void star_free(star_t *star);

/**
 * @brief Moves the star within the bounds of a window with WINDOW_WIDTH and
 * WINDOW_HEIGHT described in the star.c file. Star will bounce off all sides of
 * the window with fully elastic collisions.
 *
 * @param star the star that moves
 * @param time the time since the last update
 */
void star_move_bounce(star_t *star, double time);

/**
 * @brief Moves the star within the bounds of a window with WINDOW_WIDTH and
 * HEIGHT described in the star.c file. Star will move according to
 * GRAVITY_STRENGTH set in the star.c file and bounce off the lower height bound
 * with DAMPING_EFFECT set in star.c.
 *
 * @param star the star that moves
 * @param time the time since the last update
 */
void star_move_gravity(star_t *star, double time);

/**
 * @brief Detects if the star is out of the right most bounds of the window
 *
 * @param star the star to check
 * @return true if the star is out of bounds
 * @return false if the star is in bounds
 */
bool star_off_screen(star_t *star);

/**
 * @brief Updates the star's color as the next color in a rainbow sequence
 *
 * @param star the star with a continuously changing rainbow color
 */
void star_update_rainbow_color(star_t *star);

/**
 * @brief Draws the given star in the window.
 *
 * @param star the star to draw
 */
void star_draw(star_t *star);

#endif