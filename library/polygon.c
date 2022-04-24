#include "polygon.h"

double polygon_area(list_t *polygon)
{
    double sum = 0.0;
    size_t prev_i = list_size(polygon) - 1;
    for (size_t i = 0; i < list_size(polygon); i++)
    {
        sum += vec_cross((*(vector_t *)list_get(polygon, prev_i)), *(vector_t *)list_get(polygon, i));
        prev_i = i;
    }
    return 0.5 * sum;
}

vector_t polygon_centroid(list_t *polygon)
{
    double x_sum = 0.0;
    double y_sum = 0.0;
    size_t prev_i = list_size(polygon) - 1;

    for (size_t i = 0; i < list_size(polygon); i++)
    {
        vector_t v1 = *(vector_t *)list_get(polygon, prev_i);
        vector_t v2 = *(vector_t *)list_get(polygon, i);
        double det = vec_cross(v1, v2);
        x_sum += (v1.x + v2.x) * det;
        y_sum += (v1.y + v2.y) * det;
        prev_i = i;
    }

    double sixth_area = 1 / (polygon_area(polygon) * 6);
    return (vector_t){.x = sixth_area * x_sum, .y = sixth_area * y_sum};
}

void polygon_translate(list_t *polygon, vector_t translation)
{
    for (size_t i = 0; i < list_size(polygon); i++)
    {
        vector_t v_tmp = vec_add(*(vector_t *)list_get(polygon, i), translation);
        ((vector_t *)list_get(polygon, i))->x = v_tmp.x;
        ((vector_t *)list_get(polygon, i))->y = v_tmp.y;
    }
}

void polygon_rotate(list_t *polygon, double angle, vector_t point)
{
    polygon_translate(polygon, vec_negate(point));
    for (size_t i = 0; i < list_size(polygon); i++)
    {
        vector_t v_tmp = vec_rotate(*(vector_t *)list_get(polygon, i), angle);
        ((vector_t *)list_get(polygon, i))->x = v_tmp.x;
        ((vector_t *)list_get(polygon, i))->y = v_tmp.y;
    }
    polygon_translate(polygon, point);
}

double polygon_max_distance_across(list_t *polygon)
{
    double max = 0.0;
    vector_t c = polygon_centroid(polygon);
    for (size_t i = 0; i < list_size(polygon); i++)
    {
        double curr_dist = vec_distance(*(vector_t *)list_get(polygon, i), c);
        if (curr_dist > max)
        {
            max = curr_dist;
        }
    }
    return max;
}

double polygon_min_distance_across(list_t *polygon)
{
    double min = INFINITY;
    vector_t c = polygon_centroid(polygon);
    for (size_t i = 0; i < list_size(polygon); i++)
    {
        double curr_dist = vec_distance(*(vector_t *)list_get(polygon, i), c);
        if (curr_dist < min)
        {
            min = curr_dist;
        }
    }
    return min;
}

list_t *polygon_make_circle(vector_t position, double radius, size_t num_points)
{
    list_t *shape = list_init(num_points, free);
    for (double i = 0; i < 2 * M_PI; i += 2 * M_PI / num_points)
    {
        double x = radius * cos(i);
        double y = radius * sin(i);

        vector_t *point = malloc(sizeof(vector_t));
        point->x = position.x + x;
        point->y = position.y + y;

        list_add(shape, point);
    }
    return shape;
}

list_t *polygon_make_rectangle(double x1, double y1, double x2, double y2)
{
    list_t *shape = list_init(4, free);

    list_add(shape, vec_init(x1, y1));
    list_add(shape, vec_init(x2, y1));
    list_add(shape, vec_init(x2, y2));
    list_add(shape, vec_init(x1, y2));

    return shape;
}

list_t *polygon_make_square(vector_t position, double radius)
{
    vector_t rad_vec = (vector_t){.x = radius, .y = radius};
    vector_t lower_left = vec_subtract(position, rad_vec);
    vector_t upper_right = vec_add(position, rad_vec);
    return polygon_make_rectangle(lower_left.x, lower_left.y, upper_right.x, upper_right.y);
}

list_t *polygon_make_triangle(vector_t v1, vector_t v2, vector_t v3)
{
    list_t *shape = list_init(3, free);

    list_add(shape, vec_init(v1.x, v1.x));
    list_add(shape, vec_init(v2.x, v2.y));
    list_add(shape, vec_init(v3.x, v3.y));

    return shape;
}

void add_star_vertex(list_t *star, vector_t center, double radius, double theta) // PRIVATE
{
    double x = center.x + radius * cos(theta);
    double y = center.y - radius * sin(theta);
    vector_t *vertex = malloc(sizeof(vector_t));
    assert(vertex != NULL);
    vertex->x = x;
    vertex->y = y;
    list_add(star, vertex);
}

list_t *polygon_make_star(vector_t center, double radius, size_t points)
{
    list_t *star = list_init(points * 2, free);

    double theta = M_PI / 2;
    double delta_theta = 2 * M_PI / points;

    for (size_t i = 0; i < points; i++)
    {
        // make outer vertices
        add_star_vertex(star, center, radius, theta);

        // make inner vertices
        add_star_vertex(star, center, radius / 2, theta + delta_theta / 2);

        theta += delta_theta;
    }

    return star;
}
