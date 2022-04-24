#ifndef __PATH_H__
#define __PATH_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include "color.h"
#include "list.h"
#include "body.h"
#include "sdl_wrapper.h"
#include "forces.h"
#include "polygon.h"
#include "vector.h"
#include "scene.h"
#include "collision.h"
#include "virus.h"
#include "global_body_info.h"

/**
 * @brief path struct, stores info about the path
 *
 */

enum TURN_DIRECTION_INT
{
    LEFT_TURN,
    RIGHT_TURN,
    NO_TURN
} TURN_DIRECTION_INT;

typedef struct turn_direction
{
    int dir;
    int num; // first (0), second (1), third (2)...
} turn_direction_t;

extern const double NODE_RADIUS;

/**
 * @brief draw_simple_path, draw_medium_path, draw_complex_path
 *
 */
typedef void (*draw_path_func_t)(scene_t *scene, double demo_window_height);

/**
 * @brief Creates a path for virus particles to follow and adds it to the scene
 *
 * @param scene: pointer to the game scene
 * @param vertices: vertices of path
 * @param directions: path turn directions
 * @returns body of path created
 */
body_t *create_path(scene_t *scene, list_t *vertices, list_t *directions);

/**
 * @brief Creates a path so intricate it has a mind of its own.
 *
 * @param scene: pointer to the game scene
 */
body_t *create_path_with_a_mind_of_its_own(scene_t *scene);

/**
 * @brief Draw a simple path on scene (2 turns)
 *
 * @param scene: game scene
 * @param demo_window_height: game window height
 */
void draw_simple_path(scene_t *scene, double demo_window_height);

/**
 * @brief Draw a medium complexity path on scene
 *
 * @param scene: game scene
 * @param demo_window_height: game window height
 */
void draw_medium_path(scene_t *scene, double demo_window_height);

/**
 * @brief Draw a most complex path on scene
 *
 * @param scene: game scene
 * @param demo_window_height: game window height
 */
void draw_complex_path(scene_t *scene, double demo_window_height);

#endif // #ifndef __PATH_H__
