#ifndef GLOBAL_BODY_INFO_H
#define GLOBAL_BODY_INFO_H

#include "body.h"
#include "scene.h"
#include "sound.h"

typedef enum
{
    NOTHING,
    TOWER_TYPE,
    TOWER_RANGE_DISPLAY,
    VIRUS_TYPE,
    SHOP_TYPE,
    PATH_TYPE,
    BULLET_TYPE,
    NEXT_WAVE_BUTTON_TYPE,
    PATH_TURN_NODE_TYPE,
    WALL_TYPE,
    TOOL_TYPE,
    MOUSE_TYPE,
    BEGIN_BUTTON_TYPE,
    UPGRADE_BUTTON_TYPE,
    EASY_PATH_BUTTON_TYPE,
    MEDIUM_PATH_BUTTON_TYPE,
    HARD_PATH_BUTTON_TYPE,
    RESTART_BUTTON_TYPE,
    PLAYER_INFO_TYPE,
    HELP_BUTTON_TYPE,
    MENU_BUTTON_TYPE,
    QUIT_BUTTON_TYPE
} global_body_type_t;

typedef struct global_body_info
{
    global_body_type_t type;
    void *secondary_info;
} global_body_info_t;

typedef enum
{
    NO_SCREEN,
    WELCOME_SCREEN,
    STORY_SCREEN,
    START_GAME_SCREEN,
    PLAYING_SCREEN,
    WIN_SCREEN,
    LOSE_SCREEN
} game_screen_t;

typedef struct game_state
{
    scene_t *scene;
    int money;
    int health;
    int level;
    int score;
    global_body_type_t last_clicked_item_type; // set to nothing if background is clicked
    body_t *last_tower_selected;
    int global_shop_item_selected;   // temporary item or tower
    int specific_shop_item_selected; // type of temporary item OR type of tower
    int purchased_item_cost;
    game_screen_t screen;
    sound_t *pop_sound;
    sound_t *purchase_sound;
    sound_t *win_sound;
    sound_t *lose_sound;
    text_t *shop_description; // NULL if no description being displayed
    image_t *help_image;
    int virus_count;
} game_state_t;

/**
 * @brief Get the global body type of a body
 *
 * @param body to get body type of
 * @return body type of the selected body
 */
global_body_type_t get_global_type(body_t *body);

/**
 * @brief Get the global secondary info of a body
 *
 * @param body to get the secondary info of
 * @return secondary info about body
 */
void *get_global_secondary_info(body_t *body);

/**
 * @brief Create a global_body_info_t
 *
 * @param secondary_info a void* that can hold extra info such as a specific struct
 * associated with the global body type (eg. a tower_t for a tower body)
 * @param global_body_type the type of body
 * @return global_body_info_t*
 */
global_body_info_t *create_global_body_info(void *secondary_info, global_body_type_t global_body_type);

/**
 * @brief creates a body with a global type and info
 *
 * @param shape
 * @param mass
 * @param color
 * @param secondary_info
 * @param info_freer
 * @param type
 * @return body_t*
 */
body_t *body_init_with_secondary_info(list_t *shape, double mass, rgb_color_t color, void *secondary_info, free_func_t info_freer, global_body_type_t type);

#endif // #ifndef __GLOBAL_BODY_INFO_H__
