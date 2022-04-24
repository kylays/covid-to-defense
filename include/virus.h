#ifndef __VIRUS_H__
#define __VIRUS_H__

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
#include "global_body_info.h"
#include "path.h"

extern const int SUPER_VIRUS_HEALTH;
extern const int SUPER_VIRUS_SPAWN_COUNT;
extern const int SUPER_VIRUS_SPAWN_HEALTH;
extern const int SUPER_VIRUS_SPEED_MULTIPLIER;

/**
 * @brief Holds info about the virus.
 * radius: radius of the virus particle
 * color: color of the virus
 * speed: vector detailing covid speed
 * health: size_t detailing the health of the virus
 */
//  typedef struct virus virus_t;

typedef struct virus
{
    int health;
    double distance_travelled;
    bool is_super_virus;
    int path_nodes; // number of path nodes hit by the virus
} virus_t;

/**
 * @brief Created COVID virus particle. The color of the virus is dependent on the health value. Add virus to the scene
 *
 * @param scene: pointer to the game scene
 * @param health: health of the virus
 * @param speed: speed of the virus
 * @param position: the starting position of the virus
 * @param game_state: game state to be updated when virus out of bounds
 * @return body_t: the pointer to the coronavirus body created
 */
body_t *create_virus(scene_t *scene, int health, vector_t speed, vector_t position, game_state_t *game_state, bool is_super_virus);

/**
 * @brief Creates a right turn collision force
 *
 * @param virus the body that turns right upon hitting the path_node
 * @param path_node
 * @param axis
 * @param scene
 */
void virus_path_node_right_collision(body_t *virus, body_t *path_node, vector_t axis, scene_t *scene);

/**
 * @brief Creates a left turn collision force
 *
 * @param virus the body that turns left upon hitting the path_node
 * @param path_node
 * @param axis
 * @param aux
 */
void virus_path_node_left_collision(body_t *virus, body_t *path_node, vector_t axis, void *aux);

/**
 * @brief update virus cumulative distance travelled, to be updated each tick
 *
 * @param virus: current virus to update
 * @param dist: distance travelled since last update
*/
void virus_tick(body_t *virus_body);

//////////////////////////////////////////////////////////////////////////////////////////

// ACCESSORS

/**
 * @brief Gets the radius of the virus particle
 *
 * @param virus: pointer to the virus invader
 * @return radius of the given virus
 */
double virus_get_radius(virus_t *virus);

/**
 * @brief sets the path_nodes of a virus info
 *
 * @param virus_body
 * @param path_nodes
 */
void virus_set_path_nodes(body_t *virus_body, int path_nodes);

/**
 * @brief
 *
 * @param virus: pointer to the virus invader
 * @return int: health: current virus health;
 */
int virus_get_health(virus_t *virus);

/**
 * @brief Gets the virus color based on the virus health points.
 *
 * @param health
 * @return rgb_color_t
 */
rgb_color_t virus_get_color(int health);

/**
 * @brief Gets the distance travelled by the virus on a path
 *
 * @param virus: pointer to the virus invader
 * @return the distance traveled
 */
double virus_get_distance_travelled(virus_t *virus);

/**
 * @brief sets health
 *
 * @param virus pointer to virus to set health of
 * @param new_health new health for virus
 */
void virus_set_health(virus_t *virus, int new_health);

/**
 * @brief gets path nodes virus has passed
 *
 * @param virus pointer to virus to get the path nodes of
 * @return virus path nodes
 */
int virus_get_path_nodes(virus_t *virus);

/**
 * @brief checks whether a virus is a super virus
 *
 * @param virus pointer to virus to check
 * @return whether virus is a super virus
 */
bool virus_is_super_virus(virus_t *virus);

#endif // #ifndef __VIRUS_H__
