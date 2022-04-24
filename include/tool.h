#ifndef __TOOL_H__
#define __TOOL_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "forces.h"
#include "global_body_info.h"
#include "image.h"
#include "sound.h"
#include "text.h"

/**
 * @brief the types of towers available, stored in tower_t.id:
 * MASK: slows down viruses ($20)
 * SANITIZER: kills limited number of viruses ($50)
 */
typedef enum tool_id
{
    NOT_TOOL,
    MASK,
    SANITIZER
} tool_id_t;

/**
 * @brief Holds info about the tool.
 *
 */
typedef struct tool
{
    int price;
    collision_handler_t force;
    size_t body_count; // hand sanitizer patch -- counts how many bodies are removed
    rgb_color_t color;
    tool_id_t id;
    char *description;
} tool_t;

/**
 * The mask tool_t
 */
extern const tool_t MASK_TOOL;

/**
 * The sanitizer tool_t
 */
extern const tool_t SANITIZER_TOOL;

/**
 * @brief Create a tool and add it to the scene
 *
 * @param scene the scene that the game is in
 * @param tool_info the struct for the type of tower
 * @param position location/centroid of tower
 * @return body_t*
 */
body_t *create_tool(scene_t *scene, tool_t tool_info, vector_t position);

/**
 * @brief returns the tool struct
 *
 * @param id
 * @return tool_t from inputted id
 */
tool_t tool_get_from_id(tool_id_t id);

/**
 * @brief returns TOOL_RADIUS
 *
 * @return double
 */
double tool_get_radius();

/**
 * @brief returns string description of tool
 * 
 * @param tool_id
 * @return char* 
 */
char *tool_get_description_from_id(int id);

/**
 * @brief frees tool_info and tool_info->description
 * 
 * @param tool_info 
 */
void tool_free(tool_t *tool_info);

#endif // #ifndef __TOOL_H__