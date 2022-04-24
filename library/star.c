#include "star.h"

// STAR CONFIGURATION
const double DEFAULT_RADIUS = 80.0;
const vector_t DEFAULT_VELOCITY = {.x = 1000, .y = 0};
const double DEFAULT_ANGULAR_VELOCITY = 0.015;
const double DAMPING_EFFECT = -0.85;
const double OUTER_INNER_RATIO = 2.5;
const double GRAVITY_STRENGTH = -1500;

// COLOR CONSTS
const rgb_color_t INITIAL_COLOR = {.r = 0.0, .g = 255.0, .b = 0.0};
const double COLOR_FREQ = 0.6;
const rgb_color_t COLOR_PHASE = {.r = 0.0, .g = 2.0, .b = 4.0};
const int NUM_COLORS = 16;
const int COLOR_WIDTH = 127;
const int COLOR_CEN = 128;
const int MAX_COLOR = 255;

void scale_colors(star_t *star)
{
    star->color.r /= MAX_COLOR;
    star->color.g /= MAX_COLOR;
    star->color.b /= MAX_COLOR;
}

void unscale_colors(star_t *star)
{
    star->color.r *= MAX_COLOR;
    star->color.g *= MAX_COLOR;
    star->color.b *= MAX_COLOR;
}

void star_add_vertex(star_t *star, double radius, double theta)
{
    double x = star->center.x + radius * cos(theta);
    double y = star->center.y - radius * sin(theta);
    vector_t *vertex = malloc(sizeof(vector_t));
    assert(vertex != NULL);
    vertex->x = x;
    vertex->y = y;
    list_add(star->vertices, vertex);
}

star_t *star_init(vector_t center, vector_t velocity, double radius, size_t points, double angle,
                  rgb_color_t color)
{
    star_t *star = malloc(sizeof(star_t));
    assert(star != NULL);
    star->center = center;
    star->num_points = points;
    star->rot_angle = angle;
    star->velocity = velocity;
    star->outer_radius = radius;
    star->inner_radius = radius / OUTER_INNER_RATIO;
    star->color = color;
    star->vertices = list_init(star->num_points * 2, free);

    double theta = M_PI / 2;
    double delta_theta = 2 * M_PI / star->num_points;

    for (size_t i = 0; i < star->num_points; i++)
    {
        // make outer vertices
        star_add_vertex(star, star->outer_radius, theta);

        // make inner vertices
        star_add_vertex(star, star->inner_radius, theta + delta_theta / 2);

        theta += delta_theta;
    }

    scale_colors(star);
    return star;
}

float compute_rainbow_color(float phase, size_t num_points)
{
    return sinf((float)(COLOR_FREQ * (NUM_COLORS - num_points % NUM_COLORS) + phase)) * COLOR_WIDTH + COLOR_CEN;
}

star_t *star_init_with_n(size_t num_points)
{
    vector_t initial_center = {.x = DEFAULT_RADIUS, .y = WINDOW_HEIGHT - DEFAULT_RADIUS};

    float r = compute_rainbow_color(COLOR_PHASE.r, num_points);
    float g = compute_rainbow_color(COLOR_PHASE.g, num_points);
    float b = compute_rainbow_color(COLOR_PHASE.b, num_points);

    rgb_color_t color = {.r = r, .g = g, .b = b};

    return star_init(initial_center, DEFAULT_VELOCITY, DEFAULT_RADIUS, num_points,
                     DEFAULT_ANGULAR_VELOCITY, color);
}

void star_free(star_t *star)
{
    list_free(star->vertices);
    free(star);
}

void star_move_bounce(star_t *star, double time)
{
    vector_t displacement = vec_multiply(time, star->velocity);
    polygon_translate(star->vertices, displacement);
    star->center = vec_add(star->center, displacement);
    polygon_rotate(star->vertices, star->rot_angle, star->center);

    for (size_t i = 0; i < star->num_points * 2; i += 2)
    {
        vector_t *vertex = (vector_t *)list_get(star->vertices, i);
        if (vertex->x > WINDOW_WIDTH && star->velocity.x > 0)
        {
            star->velocity.x *= -1;
        }
        else if (vertex->x < 0 && star->velocity.x < 0)
        {
            star->velocity.x *= -1;
        }
        if (vertex->y > WINDOW_HEIGHT && star->velocity.y > 0)
        {
            star->velocity.y *= -1;
        }
        else if (vertex->y < 0 && star->velocity.y < 0)
        {
            star->velocity.y *= -1;
        }
    }
}
void star_move_gravity(star_t *star, double time)
{
    // Bounce Logic
    for (size_t i = 0; i < star->num_points * 2; i++)
    {
        vector_t *vertex = (vector_t *)list_get(star->vertices, i);
        if (vertex->y < 0 && star->velocity.y < 0)
        {
            star->velocity.y *= DAMPING_EFFECT;
            break;
        }
    }

    star->velocity.y += GRAVITY_STRENGTH * time;

    vector_t displacement = vec_multiply(time, star->velocity);
    star->center = vec_add(star->center, displacement);

    polygon_translate(star->vertices, displacement);
    polygon_rotate(star->vertices, star->rot_angle, star->center);
}

bool star_off_screen(star_t *star)
{
    for (size_t i = 0; i < list_size(star->vertices); i++)
    {
        double x = ((vector_t *)list_get(star->vertices, i))->x;
        double y = ((vector_t *)list_get(star->vertices, i))->y;
        if (x < WINDOW_WIDTH && x > 0 && y < WINDOW_HEIGHT && y > 0)
        {
            return false;
        }
    }
    return true;
}

void star_update_rainbow_color(star_t *star)
{
    // color from 0 to 255
    unscale_colors(star);
    if (star->color.r > 0 && star->color.b == 0)
    {
        star->color.r--;
        star->color.g++;
    }
    if (star->color.g > 0 && star->color.r == 0)
    {
        star->color.g--;
        star->color.b++;
    }
    if (star->color.b > 0 && star->color.g == 0)
    {
        star->color.r++;
        star->color.b--;
    }

    scale_colors(star);
}

void star_draw(star_t *star)
{
    sdl_draw_polygon(star->vertices, star->color);
}
