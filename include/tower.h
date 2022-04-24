#ifndef __TOWER_H__
#define __TOWER_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include "global_body_info.h"
#include "list.h"
#include "sdl_wrapper.h"
#include "forces.h"
#include "polygon.h"
#include "color.h"
#include "vector.h"
#include "scene.h"
#include "collision.h"
#include "body.h"
#include "virus.h"
#include "bullet.h"
#include "path.h"
#include "image.h"
#include "sound.h"
#include "text.h"

//////////////////////////////////////////// TYPEDEFS //////////////////////////////////////////////////

/**
 * @brief the types of towers available, stored in tower_t.id:
 * DART: basic shooter ($50);
 * TACK_SHOOTER: shoots bullets in multiple directions at set frequency;
 * BOMB_SHOOTER: low bullet frequency, high damage/power ($100);
 * BANK: gives bonus money each wave ($200);
 * SUPER: super tower (starts with high everything) ($1000);
 * AIRPLANE: has pursuit bullets ($800);
 * GLUE: slows down nearby COVID particles ($300)
 */
typedef enum tower_id
{
    NOT_TOWER,
    AIRPLANE_ID, // sees camo viruses
    DART_TOWER_ID,
    TACK_SHOOTER_ID, // sees camo viruses
    BOOMERANG_ID, // sees camo viruses
    BOMB_SHOOTER_ID,
    BANK_ID,
    SUPER_TOWER_ID, // sees camo viruses
    GLUE_TOWER_ID
} tower_id_t;

// stored in the upgrade_level parameter of the tower_t struct
typedef enum UPGRADES
{
    UPGRADE_ID,      // some upgrades rely on multiplying, this is a placeholder
    UPGRADE_LEVEL_1, // the tower has no upgrades when initialized
    UPGRADE_LEVEL_2,
    UPGRADE_LEVEL_3,
    UPGRADE_LEVEL_4 // max
} upgrade_t;
// UPGRADES;

/**
 * @brief stores information about a tower
 *  double range: the range a tower's bullets can reach;
 *  int upgrade_level;
 *  bullet_t *bullet_info;
 *  double time_counter;
 *  double shoot_interval: how often bullets are shot;
 *  tower_id_t tower_id;
 */
typedef struct tower
{
    int price;
    double range;
    int upgrade_level;
    bullet_t bullet_info;
    int time_counter;
    int shoot_interval;
    int bonus;
    tower_id_t id;
    vector_t flight_center;
    int bomb_time_counter;
    int bomb_interval;
    char *description;
} tower_t;

bullet_t DART_TOWER_BULLET;
tower_t DART_TOWER;
bullet_t DART_TOWER_BULLET_2;
tower_t DART_TOWER_2;
bullet_t DART_TOWER_BULLET_3;
tower_t DART_TOWER_3;
bullet_t DART_TOWER_BULLET_4;
tower_t DART_TOWER_4;

bullet_t TACK_SHOOTER_BULLET;
tower_t TACK_SHOOTER;
bullet_t TACK_SHOOTER_BULLET_2;
tower_t TACK_SHOOTER_2;
bullet_t TACK_SHOOTER_BULLET_3;
tower_t TACK_SHOOTER_3;
bullet_t TACK_SHOOTER_BULLET_4;
tower_t TACK_SHOOTER_4;

bullet_t BOMB_SHOOTER_BULLET;
tower_t BOMB_SHOOTER;
bullet_t BOMB_SHOOTER_BULLET_2;
tower_t BOMB_SHOOTER_2;
bullet_t BOMB_SHOOTER_BULLET_3;
tower_t BOMB_SHOOTER_3;
bullet_t BOMB_SHOOTER_BULLET_4;
tower_t BOMB_SHOOTER_4;
tower_t BOMB_SHOOTER_BOMB;

bullet_t SUPER_TOWER_BULLET;
tower_t SUPER_TOWER;
bullet_t SUPER_TOWER_BULLET_2;
tower_t SUPER_TOWER_2;
bullet_t SUPER_TOWER_BULLET_3;
tower_t SUPER_TOWER_3;
bullet_t SUPER_TOWER_BULLET_4;
tower_t SUPER_TOWER_4;

bullet_t AIRPLANE_BULLET;
tower_t AIRPLANE;
bullet_t AIRPLANE_BULLET_2;
tower_t AIRPLANE_2;
bullet_t AIRPLANE_BULLET_3;
tower_t AIRPLANE_3;
bullet_t AIRPLANE_BULLET_4;
tower_t AIRPLANE_4;
bullet_t AIRPLANE_BOMB_BULLET;
tower_t AIRPLANE_BOMB;

bullet_t GLUE_TOWER_BULLET;
tower_t GLUE_TOWER;
bullet_t GLUE_TOWER_BULLET_2;
tower_t GLUE_TOWER_2;
bullet_t GLUE_TOWER_BULLET_3;
tower_t GLUE_TOWER_3;
bullet_t GLUE_TOWER_BULLET_4;
tower_t GLUE_TOWER_4;

bullet_t BOOMERANG_BULLET;
tower_t BOOMERANG;
bullet_t BOOMERANG_BULLET_2;
tower_t BOOMERANG_2;
bullet_t BOOMERANG_BULLET_3;
tower_t BOOMERANG_3;
bullet_t BOOMERANG_BULLET_4;
tower_t BOOMERANG_4;

bullet_t NULL_BULLET; // all fields are 0
tower_t BANK;
tower_t BANK_2;
tower_t BANK_3;
tower_t BANK_4;

//////////////////////////////////////////// TOWER FUNCTIONS //////////////////////////////////////////////////

/**
 * @brief Set the up specific tower structs; call at the beginning of the game
 */
void setup_tower_configs();

/**
 * @brief returns a tower struct with given info inside
 *
 * @param price
 * @param range
 * @param upgrade_level
 * @param bullet_info
 * @param time_counter
 * @param shoot_interval
 * @param bonus
 * @param id
 * @return tower_t
 */
tower_t tower_make_struct(int price, double range, int upgrade_level, bullet_t bullet_info,
                          int time_counter, int shoot_interval, int bonus,
                          tower_id_t id, char *description);

/**
 * @brief returns an airplane tower struct with given info inside
 *
 * @param price
 * @param range
 * @param upgrade_level
 * @param bullet_info
 * @param time_counter
 * @param shoot_interval
 * @param bonus
 * @param id
 * @param bomb_time_counter
 * @param bomb_interval
 * @return tower_t
 */
tower_t tower_make_airplane_struct(int price, double range, int upgrade_level, bullet_t bullet_info,
                                   int time_counter, int shoot_interval, int bonus, tower_id_t id,
                                   int bomb_time_counter, int bomb_interval, char *description);

/**
 * @brief Create a tower and add it to the scene
 *
 * @param scene the scene that the game is in
 * @param tower_info the struct for the type of tower
 * @param position location/centroid of tower
 * @return body_t*
 */
body_t *create_tower(scene_t *scene, tower_t tower_info, vector_t position);

/**
 * @brief updates a tower
 *
 * @param tower_body
 * @param scene
 * @param game_state
 */
void tower_tick(body_t *tower_body, scene_t *scene, game_state_t *game_state);

/**
 * @brief updates all towers in the scene, should be called every scene tick
 *
 * @param scene
 * @param game_state
 */
void tower_tick_all_towers(scene_t *scene, game_state_t *game_state);

/**
 * @brief displays the tower range, should be called when the tower is clicked
 * @param tower_body the tower clicked
 * @param scene the tower is in
 */
void tower_display_range(body_t *tower_body, scene_t *scene);

/**
 * @brief undisplays the tower range, should be called when the tower is unclicked
 *
 * @param scene
 */
void tower_undisplay_range(scene_t *scene);

/**
 * @brief frees tower_info and tower_info->description
 *
 * @param tower_info
 */
void tower_free(tower_t *tower_info);

////////////////////////////////////////////////////////////////////////////////////////////

// ACCESSORS

/**
 * @brief returns TOWER_RADIUS
 *
 * @return double
 */
double tower_get_radius();

/**
 * @brief returns the type of tower given a body
 *
 * @param tower_body
 * @return tower_id_t
 */
tower_id_t tower_get_type(body_t *tower_body);

/**
 * @brief Maps tower ids to initial tower config tower_t's (upgrade_level_1)
 *
 * @param tower_id
 * @return tower_t
 */
tower_t tower_get_from_id(tower_id_t tower_id);

/**
 * @brief Maps tower ids to tower config tower_t's
 *
 * @param tower_id
 * @param upgrade_level
 * @return tower_t
 */
tower_t tower_get_from_id_and_level(int tower_id, int upgrade_level);

/**
 * @brief returns the offset tower color
 *
 * @param id
 * @return rgb_color_t
 */
rgb_color_t tower_get_color(tower_id_t id);

/**
 * @brief returns AIRPLANE_FLIGHT_RADIUS
 *
 * @return double
 */
double tower_get_airplane_flight_radius();

/**
 * @brief returns string description of tower
 *
 * @param tower
 * @return char*
 */
char *tower_get_description_from_id(tower_id_t id);

#endif // #ifndef __TOWER_H__
