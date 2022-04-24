#include "body.h"

const double SIZE_MULTIPLIER = 0.9;

typedef struct body
{
    list_t *shape;
    double mass;
    rgb_color_t color;
    vector_t velocity;
    vector_t centroid;
    double angle;
    vector_t forces;
    vector_t impulses;
    bool remove_flag;
    void *info;
    free_func_t info_freer;
    double size;    // stores the max distance across the polygon shape
    image_t *image; // sprite image that moves with the body, use body_set_image(body, image) after init
    text_t *label;  // a rendered text label, eg. a button called "next wave"
} body_t;

body_t *body_init(list_t *shape, double mass, rgb_color_t color)
{
    return body_init_with_info(shape, mass, color, NULL, NULL);
}

body_t *body_init_with_info_with_image(list_t *shape, double mass, rgb_color_t color, void *info,
                                       free_func_t info_freer, char *image_filename, int image_type,
                                       vector_t image_size)
{
    body_t *body = body_init_with_info(shape, mass, color, info, info_freer);
    body->image = image_init(image_filename, body->centroid,
                             vec_multiply(SIZE_MULTIPLIER, image_size), image_type);
    return body;
}

body_t *body_init_with_info_with_label(list_t *shape, double mass, rgb_color_t body_color,
                                       void *info, free_func_t info_freer, char *text,
                                       rgb_color_t text_color, TTF_Font *font, vector_t text_size)
{
    body_t *body = body_init_with_info(shape, mass, body_color, info, info_freer);
    body->label = text_init(text, body->centroid,
                            vec_multiply(SIZE_MULTIPLIER, text_size), text_color, font);
    return body;
}

body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color, void *info, free_func_t info_freer)
{
    body_t *b = malloc(sizeof(body_t));
    assert(b != NULL);
    b->shape = shape;
    b->mass = mass;
    b->color = color;
    b->velocity = VEC_ZERO;
    b->centroid = polygon_centroid(shape);
    b->angle = 0.0;
    b->forces = VEC_ZERO;
    b->impulses = VEC_ZERO;
    b->remove_flag = false;
    b->info = info;
    b->info_freer = info_freer;
    b->size = polygon_max_distance_across(shape);
    b->image = NULL;
    b->label = NULL;
    return b;
}

void body_free(body_t *body)
{
    list_free(body->shape);
    if (body->info_freer != NULL)
    {
        body->info_freer(body->info);
    }
    if (body->image)
    {
        image_remove(body->image);
    }
    if (body->label)
    {
        text_remove(body->label);
    }
    body = NULL;
    free(body);
}

double body_get_size(body_t *body)
{
    return body->size;
}

list_t *body_get_shape(body_t *body)
{
    size_t shape_size = list_size(body->shape);
    list_t *shape = list_init(shape_size, free);
    for (size_t i = 0; i < shape_size; i++)
    {
        vector_t *old_point = list_get(body->shape, i);
        vector_t *new_point = malloc(sizeof(vector_t));
        assert(new_point != NULL);
        new_point->x = old_point->x;
        new_point->y = old_point->y;
        list_add(shape, new_point);
    }
    return shape;
}

vector_t body_get_centroid(body_t *body)
{
    return body->centroid;
}

vector_t body_get_forces(body_t *body)
{
    return body->forces;
}

vector_t body_get_impulses(body_t *body)
{
    return body->impulses;
}

vector_t body_get_velocity(body_t *body)
{
    return body->velocity;
}

double body_get_mass(body_t *body)
{
    return body->mass;
}

void body_set_mass(body_t *body, double new_mass)
{
    body->mass = new_mass;
}

rgb_color_t body_get_color(body_t *body)
{
    return body->color;
}

void *body_get_info(body_t *body)
{
    return body->info;
}

void body_set_info(body_t *body, void *info)
{
    body->info = info;
}

void body_set_centroid(body_t *body, vector_t x)
{
    vector_t change = vec_subtract(x, body->centroid);
    polygon_translate(body->shape, change);
    body->centroid = vec_add(body->centroid, change);
}

void body_set_velocity(body_t *body, vector_t v)
{
    body->velocity = v;
}

void body_set_rotation(body_t *body, double angle)
{
    polygon_rotate(body->shape, angle - body->angle, body->centroid);
    body->angle = angle;
}

void body_add_force(body_t *body, vector_t force)
{
    body->forces = vec_add(body->forces, force);
}

void body_add_impulse(body_t *body, vector_t impulse)
{
    body->impulses = vec_add(body->impulses, impulse);
}

void body_tick(body_t *body, double dt)
{
    vector_t old_vel = body->velocity;
    body->velocity = vec_add(body->velocity, vec_multiply(dt / body->mass, body->forces));
    body->forces = VEC_ZERO;
    body->velocity = vec_add(body->velocity, vec_multiply(1.0 / body->mass, body->impulses));
    body->impulses = VEC_ZERO;
    vector_t avg_vel = vec_multiply(0.5, vec_add(old_vel, body->velocity));
    vector_t displacement = vec_multiply(dt, avg_vel);
    polygon_translate(body->shape, displacement);
    body->centroid = vec_add(body->centroid, displacement);
    if (body->image)
    {
        image_set_position(body->image, body->centroid);
    }
    if (body->label)
    {
        text_set_position(body->label, body->centroid);
    }
}

void body_remove(body_t *body)
{
    body->remove_flag = true;
    if (body->image)
    {
        image_remove(body->image);
    }
    if (body->label)
    {
        text_remove(body->label);
    }
}

bool body_is_removed(body_t *body)
{
    return body->remove_flag;
}

void body_set_color(body_t *body, rgb_color_t color)
{
    body->color = color;
}

image_t *body_get_image(body_t *body)
{
    return (body->image);
}

void body_set_image(body_t *body, image_t *image)
{
    body->image = image;
}

text_t *body_get_label(body_t *body)
{
    return (body->label);
}

// void body_set_label(body_t *body, text_t *label)
void body_set_label(body_t *body, char *label)
{
    text_set(body->label, label);
    // body->label = label;
}
