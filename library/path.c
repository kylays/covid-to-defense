#include "path.h"

const double PATH_MASS = INFINITY;
const double PATH_WIDTH = 20;
// for outer/inner
const int PATH_EDGE_MIN = 0;
const int PATH_EDGE_MAX = 800; // value based on shop
const rgb_color_t PATH_COLOR = {.r = (float)0.9, .g = (float)0.91, .b = (float)0.83};
const double NODE_RADIUS = 3.0;
const size_t NODE_POINTS = 4;

body_t *create_path(scene_t *scene, list_t *vertices, list_t *directions)
{
    // overall path
    body_t *path_body = body_init_with_secondary_info(vertices, PATH_MASS, PATH_COLOR, NULL, NULL, PATH_TYPE);
    scene_add_body(scene, path_body);

    // create path turn nodes, add collision is in create_virus()
    for (size_t i = 0; i < list_size(vertices) / 2; i++)
    {
        // inner always bigger than outer
        vector_t v1 = *((vector_t *)list_get(vertices, i));                           // outer
        vector_t v2 = *((vector_t *)list_get(vertices, list_size(vertices) - 1 - i)); // inner
        vector_t midpoint = vec_midpoint(v1, v2);                                     // midpoint

        turn_direction_t *dir_tmp = ((turn_direction_t *)list_get(directions, i));
        turn_direction_t *dir_info = malloc(sizeof(turn_direction_t));
        dir_info->dir = dir_tmp->dir;
        dir_info->num = (int)i;

        list_t *node_circle = polygon_make_circle(midpoint, NODE_RADIUS, NODE_POINTS);
        body_t *node = body_init_with_secondary_info(node_circle, PATH_MASS, PATH_COLOR, dir_info, free, PATH_TURN_NODE_TYPE);
        scene_add_body(scene, node);
    }

    return path_body;
}

void draw_simple_path(scene_t *scene, double demo_window_height)
{
    list_t *path_vertices = list_init(8, free);

    // outer
    list_add(path_vertices, vec_init(PATH_EDGE_MIN, demo_window_height - 4 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 2, demo_window_height - 4 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 2, PATH_EDGE_MIN + 2 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MIN, PATH_EDGE_MIN + 2 * PATH_WIDTH));

    // inner
    list_add(path_vertices, vec_init(PATH_EDGE_MIN, PATH_EDGE_MIN + 3 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 3, PATH_EDGE_MIN + 3 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 3, demo_window_height - 5 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MIN, demo_window_height - 5 * PATH_WIDTH));

    list_t *directions = list_init(4, free);
    turn_direction_t *l = malloc(sizeof(turn_direction_t));
    l->dir = LEFT_TURN;
    turn_direction_t *r = malloc(sizeof(turn_direction_t));
    r->dir = RIGHT_TURN;
    turn_direction_t *n = malloc(sizeof(turn_direction_t));
    n->dir = NO_TURN;
    list_add(directions, n);
    list_add(directions, r);
    list_add(directions, r);
    list_add(directions, n);

    create_path(scene, path_vertices, directions);
}

void draw_medium_path(scene_t *scene, double demo_window_height)
{
    list_t *path_vertices = list_init(16, free);

    // outer
    list_add(path_vertices, vec_init(PATH_EDGE_MIN, demo_window_height - 4 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 2, demo_window_height - 4 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 2, demo_window_height - 9 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MIN + PATH_WIDTH * 5, demo_window_height - 9 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MIN + PATH_WIDTH * 5, demo_window_height - 12 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 2, demo_window_height - 12 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 2, PATH_EDGE_MIN + 2 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MIN, PATH_EDGE_MIN + 2 * PATH_WIDTH));

    // inner
    list_add(path_vertices, vec_init(PATH_EDGE_MIN, PATH_EDGE_MIN + 3 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 3, PATH_EDGE_MIN + 3 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 3, demo_window_height - 13 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MIN + PATH_WIDTH * 4, demo_window_height - 13 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MIN + PATH_WIDTH * 4, demo_window_height - 8 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 3, demo_window_height - 8 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 3, demo_window_height - 5 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MIN, demo_window_height - 5 * PATH_WIDTH));

    list_t *directions = list_init(8, free);
    turn_direction_t *l = malloc(sizeof(turn_direction_t));
    l->dir = LEFT_TURN;
    turn_direction_t *r = malloc(sizeof(turn_direction_t));
    r->dir = RIGHT_TURN;
    turn_direction_t *n = malloc(sizeof(turn_direction_t));
    n->dir = NO_TURN;
    list_add(directions, n);
    list_add(directions, r);
    list_add(directions, r);
    list_add(directions, l);
    list_add(directions, l);
    list_add(directions, r);
    list_add(directions, r);
    list_add(directions, n);

    create_path(scene, path_vertices, directions);
}

void draw_complex_path(scene_t *scene, double demo_window_height)
{
    list_t *path_vertices = list_init(24, free);

    // outer
    list_add(path_vertices, vec_init(PATH_EDGE_MIN, demo_window_height - 4 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX / 3, demo_window_height - 4 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX / 3, demo_window_height - 8 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX / 3 * 2, demo_window_height - 8 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX / 3 * 2, demo_window_height - 4 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 2, demo_window_height - 4 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 2, demo_window_height - 15 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MIN + PATH_WIDTH * 5, demo_window_height - 15 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MIN + PATH_WIDTH * 5, demo_window_height - 18 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 2, demo_window_height - 18 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 2, PATH_EDGE_MIN + 2 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MIN, PATH_EDGE_MIN + 2 * PATH_WIDTH));

    // inner
    list_add(path_vertices, vec_init(PATH_EDGE_MIN, PATH_EDGE_MIN + 3 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 3, PATH_EDGE_MIN + 3 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 3, demo_window_height - 19 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MIN + PATH_WIDTH * 4, demo_window_height - 19 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MIN + PATH_WIDTH * 4, demo_window_height - 14 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 3, demo_window_height - 14 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX - PATH_WIDTH * 3, demo_window_height - 5 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX / 3 * 2 + PATH_WIDTH, demo_window_height - 5 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX / 3 * 2 + PATH_WIDTH, demo_window_height - 9 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX / 3 - PATH_WIDTH, demo_window_height - 9 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MAX / 3 - PATH_WIDTH, demo_window_height - 5 * PATH_WIDTH));
    list_add(path_vertices, vec_init(PATH_EDGE_MIN, demo_window_height - 5 * PATH_WIDTH));

    list_t *directions = list_init(12, free);
    turn_direction_t *l = malloc(sizeof(turn_direction_t));
    l->dir = LEFT_TURN;
    turn_direction_t *r = malloc(sizeof(turn_direction_t));
    r->dir = RIGHT_TURN;
    turn_direction_t *n = malloc(sizeof(turn_direction_t));
    n->dir = NO_TURN;
    list_add(directions, n);
    list_add(directions, r);
    list_add(directions, l);
    list_add(directions, l);
    list_add(directions, r);
    list_add(directions, r);
    list_add(directions, r);
    list_add(directions, l);
    list_add(directions, l);
    list_add(directions, r);
    list_add(directions, r);
    list_add(directions, n);

    create_path(scene, path_vertices, directions);
}
