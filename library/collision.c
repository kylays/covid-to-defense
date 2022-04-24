#include "collision.h"

typedef struct
{
    double min;
    double max;
} projection_range_t;

// PRIVATE HELPER FUNCTION DECLARATIONS
list_t *get_projection_axes(list_t *shapes);
projection_range_t get_projection_range(list_t *shape, vector_t *axis);

list_t *get_projection_axes(list_t *shapes)
{
    assert(list_size(shapes) == 2);
    list_t *axes = list_init(list_size(list_get(shapes, 0)) + list_size(list_get(shapes, 1)), (free_func_t)free);

    for (size_t s = 0; s < list_size(shapes); s++)
    {
        list_t *shape = list_get(shapes, s);
        for (size_t v1 = 0; v1 < list_size(shape); v1++)
        {
            // get adjacent edge, making sure to wrap around at the end
            size_t v2 = (v1 + 1) % (list_size(shape) - 1);

            // getting the perpendicular unit axis
            vector_t edge = vec_unit(vec_subtract(*((vector_t *)list_get(shape, v1)),
                                                  *((vector_t *)list_get(shape, v2))));
            edge = vec_rotate(edge, M_PI / 2);
            vector_t *axis = malloc(sizeof(vector_t));
            axis->x = edge.x;
            axis->y = edge.y;
            list_add(axes, axis);
        }
    }
    return axes;
}

projection_range_t get_projection_range(list_t *shape, vector_t *axis)
{
    double min_projection = HUGE_VAL;
    double max_projection = -HUGE_VAL;

    // looping through the shape's vertices
    for (size_t i = 0; i < list_size(shape); i++)
    {
        vector_t vertex = *((vector_t *)list_get(shape, i));
        double projection = vec_dot(vertex, *axis);

        if (projection < min_projection)
        {
            min_projection = projection;
        }
        if (projection > max_projection)
        {
            max_projection = projection;
        }
    }
    return (projection_range_t){.min = min_projection, .max = max_projection};
}

collision_info_t find_collision(list_t *shape1, list_t *shape2)
{
    list_t *shapes = list_init(2, NULL);
    list_add(shapes, shape1);
    list_add(shapes, shape2);
    list_t *axes = get_projection_axes(shapes);

    bool collided = true;
    double min_overlap = HUGE_VAL;
    vector_t collision_axis = *((vector_t *)list_get(axes, 0));

    for (size_t i = 0; i < list_size(axes); i++)
    {
        vector_t *axis = list_get(axes, i);
        projection_range_t projection1 = get_projection_range(shape1, axis);
        projection_range_t projection2 = get_projection_range(shape2, axis);

        double min1 = projection1.min;
        double max1 = projection1.max;
        double min2 = projection2.min;
        double max2 = projection2.max;

        // two overlap possibilities depending on orientation of shapes relative to each other
        double overlap12 = fabs(max2 - min1);
        double overlap21 = fabs(max1 - min2);
        double min = overlap12 < overlap21 ? overlap12 : overlap21;

        if (min < min_overlap)
        {
            collision_axis.x = axis->x;
            collision_axis.y = axis->y;
            min_overlap = min;
        }

        if (min2 > max1 || max2 < min1) // the projections don't overlap
        {
            collided = false;
        }
    }

    free(shapes);
    collision_info_t collision_info = (collision_info_t){.collided = collided,
                                                         .axis = collision_axis};
    return collision_info;
}
