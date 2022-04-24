#include "global_body_info.h"

global_body_type_t get_global_type(body_t *body)
{
    if (body_get_info(body))
    {
        return ((global_body_info_t *)body_get_info(body))->type;
    }
    return NOTHING;
}

void *get_global_secondary_info(body_t *body)
{
    return ((global_body_info_t *)body_get_info(body))->secondary_info;
}

global_body_info_t *create_global_body_info(void *secondary_info, global_body_type_t global_body_type)
{

    global_body_info_t *t = malloc(sizeof(global_body_info_t));
    *t = (global_body_info_t){.secondary_info = secondary_info, .type = global_body_type};
    return t;
}

body_t *body_init_with_secondary_info(list_t *shape, double mass, rgb_color_t color,
                                      void *secondary_info, free_func_t info_freer, global_body_type_t type)
{
    return body_init_with_info(shape, mass, color, create_global_body_info(secondary_info, type), info_freer);
}
