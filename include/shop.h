#ifndef __SHOP_H__
#define __SHOP_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include "sdl_wrapper.h"
#include "vector.h"
#include "list.h"
#include "vector.h"
#include "body.h"
#include "polygon.h"
#include "forces.h"
#include "scene.h"
#include "collision.h"
#include "color.h"
#include "tower.h"
#include "image.h"
#include "sound.h"
#include "text.h"

typedef struct shop_item shop_item_t;

/**
 * @brief initialize a shop item

 * @param cost: cost of the item
 * @param global_type: type of item (ex. tower)
 * @param specific_type: specific type (ex. dart tower)
 * @returns the new shop_item_t created
*/
shop_item_t *shop_item_init(int cost, global_body_type_t global_type, int specific_type, char *description);

/**
 * @brief Creates shop of all items
 *
 * @param scene scene to add shop to
 */
void create_shop(scene_t *scene);

/**
 * @brief After checking there is enough purchasing money,
 * purchases the item, decreasing from player budget and storing
 * item purchased in game state to be placed by mouse.
 *
 * @param game_state_t game state storing player budget and mouse info
 * @param shop_item_body item to be purchased
 * @returns price of item purchased
 */
int purchase_item(game_state_t *game_state, body_t *shop_item_body);

/**
 * @brief when tower is selected, displays available upgrades for the selected tower
 *
 * @param tower_body: pointer to the user selected tower
 */
void shop_purchase_tower_upgrade(body_t *tower_body, game_state_t *game_state);

/**
 * @brief displays the upgrade tower button when an upgradeable tower is clicked on; called in mouse_handler
 *
 * @param tower_body
 * @param game_state
 * @return void
 */
void shop_display_upgrade_button(body_t *tower_body, game_state_t *game_state);

/**
 * @brief removes the upgrade button
 *
 * @param scene
 */
void tower_undisplay_upgrade_button(scene_t *scene);

/**
 * @brief To be used when an item is placed. Resets the game state which stores last clicked shop item, cost, etc.
 *
 * @param game_state_t game state storing player budget and mouse info
 */
void shop_reset_game_state(game_state_t *game_state, bool successful_purchase);

/**
 * @brief Place an item after it has been purchased depending on mouse position
 *
 * @param game_state_t game state storing player budget and mouse info
 * @param mouse_pos position of the mouse
 * @param demo_playing_width width of playing window
 * @param demo_playing_height height of playing window
 */
void shop_place_item(game_state_t *game_state, vector_t mouse_pos, double demo_playing_width, double demo_playing_height);

/**
 * @brief temporarily displays position of where a player might place an item
 *
 * @param game_state_t game state storing player budget and mouse info
 * @param mouse_pos position of the mouse
 * @param demo_playing_width width of playing window
 * @param demo_playing_height height of playing window
 */
void shop_temp_display(game_state_t *game_state, vector_t mouse_pos, double demo_playing_width, double demo_playing_height);

/**
 * @brief renders the shop item description
 * 
 * @param game_state 
 * @param body 
 */
void shop_display_description(game_state_t *game_state, body_t *body);

/**
 * @brief removes shop item description from scene by marking the description for removal
 * 
 * @param game_state 
 */
void shop_undisplay_description(game_state_t *game_state);

/**
 * @brief displays the help image when help button pressed
 * 
 * @param game_state 
 */
void display_help(game_state_t *game_state);

/**
 * @brief removes help image from screen by marking it for removal
 * 
 * @param game_state 
 */
void undisplay_help(game_state_t *game_state);

#endif // #ifndef __SHOP_H__
