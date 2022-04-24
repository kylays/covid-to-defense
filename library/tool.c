#include "tool.h"

const double TOOL_RADIUS = 12; // slightly bigger than path
const double TOOL_MASS = INFINITY;
const double TOOL_SPEED = 0;
const double MASK_SLOWDOWN_FACTOR = 500;

void virus_sanitizer_collision(body_t *virus, body_t *sanitizer, vector_t axis, scene_t *scene);
void virus_mask_collision(body_t *virus, body_t *mask, vector_t axis, scene_t *scene);

const tool_t MASK_TOOL = {.price = 20, .force = (collision_handler_t)virus_mask_collision, .body_count = 5, .color = {.r = (float)0.57, .g = (float)0.73, .b = (float)1}, .id = MASK, .description = "Mask: slows down 5 viruses"};
const tool_t SANITIZER_TOOL = {.price = 50, .force = (collision_handler_t)virus_sanitizer_collision, .body_count = 8, .color = {.r = (float)0.8, .g = (float).57, .b = (float)0.98}, .id = SANITIZER, .description = "Hand Sanitizer: kills 8 viruses"};

// physics collision
void virus_mask_collision(body_t *virus, body_t *mask, vector_t axis, scene_t *scene)
{
    tool_t *mask_info = get_global_secondary_info(mask);
    if (mask_info->body_count > 0)
    {
        vector_t force = vec_negate(vec_multiply(MASK_SLOWDOWN_FACTOR * body_get_mass(virus),
                                                 vec_unit(body_get_velocity(virus))));
        body_add_force(virus, force);
        mask_info->body_count--;
    }
    else
    {
        body_remove(mask);
    }
}

void virus_sanitizer_collision(body_t *virus, body_t *sanitizer, vector_t axis, scene_t *scene)
{
    tool_t *sanitizer_info = get_global_secondary_info(sanitizer);
    if (sanitizer_info->body_count > 0)
    {
        body_remove(virus);
        sanitizer_info->body_count--;
    }
    else
    {
        body_remove(sanitizer);
    }
}

body_t *create_tool(scene_t *scene, tool_t tool_info, vector_t position)
{
    tool_t *tool = malloc(sizeof(tool_t));
    assert(tool != NULL);
    *tool = tool_info;
    global_body_info_t *global_info = create_global_body_info(tool, TOOL_TYPE);
    vector_t size = (vector_t){TOOL_RADIUS * 2, TOOL_RADIUS * 2};
    list_t *tool_shape = polygon_make_square(position, TOOL_RADIUS * 0.5);

    char *filename = NULL;
    if (tool->id == MASK)
    {
        filename = "images/mask.png";
        // adjust size: make wider and not taller
        size = vec_add(size, (vector_t){.x = 2 * TOOL_RADIUS, .y = 2.6 * TOOL_RADIUS}); // oriented vertically
    }
    else if (tool->id == SANITIZER)
    {
        filename = "images/handsanitizer.png";
        // adjust size: make taller and not wider
        size = vec_add(size, (vector_t){.x = TOOL_RADIUS, .y = 2.2 * TOOL_RADIUS});
    }

    body_t *body = body_init_with_info_with_image(tool_shape, TOOL_MASS, tool_get_from_id(tool->id).color,
                                                  global_info, (free_func_t) tool_free, filename, IMG_INIT_PNG, size);
    scene_add_body(scene, body);

    for (size_t i = 0; i < scene_bodies(scene); i++)
    {
        body_t *virus_body = scene_get_body(scene, i);
        if (get_global_type(virus_body) == VIRUS_TYPE)
        {
            collision_handler_t force = tool_info.force;
            create_collision(scene, virus_body, body, force, scene, NULL);
        }
    }
    return body;
}

double tool_get_radius()
{
    return TOOL_RADIUS;
}

tool_t tool_get_from_id(tool_id_t id)
{
    if (id == MASK)
    {
        return MASK_TOOL;
    }
    else if (id == SANITIZER)
    {
        return SANITIZER_TOOL;
    }
    return SANITIZER_TOOL;
}

char *tool_get_description_from_id(int id)
{
    return tool_get_from_id(id).description;
}

void tool_free(tool_t *tool_info)
{
    //free(tool_info->description);
    free(tool_info);
}
