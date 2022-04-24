#include <assert.h>
#include "vector.h"

const vector_t VEC_ZERO = {.x = 0, .y = 0};

vector_t *vec_init(double x, double y)
{
    vector_t *v = malloc(sizeof(vector_t));
    assert(v != NULL);
    v->x = x;
    v->y = y;
    return v;
}

vector_t vec_add(vector_t v1, vector_t v2)
{
    return (vector_t){.x = v1.x + v2.x, .y = v1.y + v2.y};
}

vector_t vec_subtract(vector_t v1, vector_t v2)
{
    return vec_add(v1, vec_negate(v2));
}

vector_t vec_negate(vector_t v)
{
    return vec_multiply(-1, v);
}

vector_t vec_multiply(double scalar, vector_t v)
{
    return (vector_t){.x = scalar * v.x, .y = scalar * v.y};
}

double vec_dot(vector_t v1, vector_t v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

double vec_cross(vector_t v1, vector_t v2)
{
    return v1.x * v2.y - v1.y * v2.x;
}

vector_t vec_rotate(vector_t v, double angle)
{
    return (vector_t){.x = v.x * cos(angle) - v.y * sin(angle),
                      .y = v.x * sin(angle) + v.y * cos(angle)};
}

double vec_magnitude(vector_t v)
{
    return sqrt(vec_dot(v, v));
}

double vec_distance(vector_t v1, vector_t v2)
{
    return vec_magnitude(vec_subtract(v1, v2));
}

vector_t vec_unit(vector_t v)
{
    return vec_multiply(1.0 / vec_magnitude(v), v);
}

vector_t vec_midpoint(vector_t v1, vector_t v2)
{
    return vec_multiply(0.5, vec_add(v1, v2));
}
