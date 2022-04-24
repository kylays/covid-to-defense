#include "virus.h"
#include "tool.h"

/////////////////////////////////// CONSTS & STRUCTS ////////////////////////////////

const double VIRUS_MASS = 100;
const size_t VIRUS_CIRCLE_PTS = 5;
const double VIRUS_CIRCLE_RADIUS = 7.5;
const vector_t VIRUS_INITIAL_VELOCITY = {.x = 85.0, .y = 0.0};
const size_t VIRUS_COLOR_MULTIPLIER = 5;
const vector_t SUPER_IMG_SIZE = {.x = 100.0, .y = 70.0};

const int SUPER_VIRUS_HEALTH = 19;
const int SUPER_VIRUS_SPAWN_COUNT = 10;
const int SUPER_VIRUS_SPAWN_HEALTH = 20;
const int SUPER_VIRUS_SPEED_MULTIPLIER = 1;

///////////////////////////////////// FUNTIONS  //////////////////////////////////

// physics collision
void virus_wall_collision(body_t *virus, body_t *wall, vector_t axis, game_state_t *game_state) // private
{
    // Check walls
    vector_t wall_position = body_get_centroid(wall);
    vector_t virus_vel = body_get_velocity(virus);
    if ((wall_position.y < 200 && virus_vel.y < 0) || (wall_position.y > 200 && virus_vel.y > 0) ||
        (wall_position.x < 200 && virus_vel.x < 0) || (wall_position.x > 200 && virus_vel.x > 0))
    {
        game_state->health -= (int)(((virus_t *)get_global_secondary_info(virus))->health);
        body_remove(virus);
    }
}

body_t *create_virus(scene_t *scene, int health, vector_t speed, vector_t position,
                     game_state_t *game_state, bool is_super_virus)
{
    virus_t *virus = malloc(sizeof(virus_t));
    assert(virus != NULL);
    virus->health = health;
    virus->distance_travelled = 0;
    virus->is_super_virus = is_super_virus;
    virus->path_nodes = 0;

    //create virus
    list_t *shape = polygon_make_circle(position, VIRUS_CIRCLE_RADIUS, VIRUS_CIRCLE_PTS);
    body_t *virus_body = body_init_with_secondary_info(shape, VIRUS_MASS,
                                                       virus_get_color(health), virus, free, VIRUS_TYPE);
    if (is_super_virus)
    {
        image_t *image = image_init("images/virus.png", position, SUPER_IMG_SIZE, IMG_INIT_PNG);
        body_set_image(virus_body, image);
    }
    body_set_velocity(virus_body, speed);
    scene_add_body(scene, virus_body);

    // create collision force with turn nodes
    for (size_t i = 0; i < scene_bodies(scene); i++)
    {
        body_t *curr_body = scene_get_body(scene, i);
        if (get_global_type(curr_body) == PATH_TURN_NODE_TYPE)
        {
            turn_direction_t *dir_info = (turn_direction_t *)get_global_secondary_info(curr_body);
            if (dir_info->dir == RIGHT_TURN)
            {
                create_collision(scene, virus_body, curr_body,
                                 (collision_handler_t)virus_path_node_right_collision, scene, NULL);
            }
            else if (dir_info->dir == LEFT_TURN)
            {
                create_collision(scene, virus_body, curr_body,
                                 (collision_handler_t)virus_path_node_left_collision, scene, NULL);
            }
        }
        else if (get_global_type(curr_body) == WALL_TYPE)
        {
            create_collision(scene, virus_body, curr_body,
                             (collision_handler_t)virus_wall_collision, game_state, NULL);
        }
        else if (get_global_type(curr_body) == TOOL_TYPE)
        {
            tool_t *info = get_global_secondary_info(curr_body);
            create_collision(scene, virus_body, curr_body,
                             (collision_handler_t)info->force, scene, NULL);
        }
    }

    return virus_body;
}

bool check_should_turn(body_t *virus, body_t *path_node)
{
    // compare virus number of turns with path node's id
    virus_t *virus_info = (virus_t *)get_global_secondary_info(virus);
    turn_direction_t *node_info = (turn_direction_t *)get_global_secondary_info(path_node);
    printf("virus: %d, turn: %d, dir: %d \n", virus_info->path_nodes, node_info->num, node_info->dir);
    return virus_info->path_nodes + 1 == node_info->num;
}

void virus_path_node_right_collision(body_t *virus, body_t *path_node, vector_t axis, scene_t *scene)
{
    if (check_should_turn(virus, path_node))
    {
        ((virus_t *)get_global_secondary_info(virus))->path_nodes += 1;
        vector_t old_vel = body_get_velocity(virus);

        vector_t diff = vec_subtract(body_get_centroid(path_node), body_get_centroid(virus));
        body_set_centroid(virus, vec_add(body_get_centroid(virus), diff));

        vector_t new_vel;
        if (old_vel.y == 0)
        {
            new_vel = (vector_t){.x = old_vel.y, .y = -1 * old_vel.x};
            body_set_velocity(virus, new_vel);
        }

        else
        {
            new_vel = (vector_t){.x = old_vel.y, .y = old_vel.x};
            body_set_velocity(virus, new_vel);
        }
        body_set_velocity(virus, new_vel);
    }
}

void virus_path_node_left_collision(body_t *virus, body_t *path_node, vector_t axis, void *aux)
{
    if (check_should_turn(virus, path_node))
    {
        ((virus_t *)get_global_secondary_info(virus))->path_nodes += 1;
        vector_t old_vel = body_get_velocity(virus);

        vector_t diff = vec_subtract(body_get_centroid(path_node), body_get_centroid(virus));
        body_set_centroid(virus, vec_add(body_get_centroid(virus), diff));

        vector_t new_vel;
        if (old_vel.x != 0)
        {
            new_vel = (vector_t){.x = old_vel.y, .y = old_vel.x};
        }
        else
        {
            new_vel = (vector_t){.x = -1 * old_vel.y, .y = old_vel.x};
        }

        body_set_velocity(virus, new_vel);
    }
}

void virus_tick(body_t *virus_body)
{
    virus_t *virus = get_global_secondary_info(virus_body);
    virus->distance_travelled += vec_magnitude(body_get_velocity(virus_body));
}

//////////////////////////////// ACCESSORS ////////////////////////////////

int virus_get_health(virus_t *virus)
{
    return virus->health;
}

double virus_get_distance_travelled(virus_t *virus)
{
    return virus->distance_travelled;
}

void virus_set_health(virus_t *virus, int new_health)
{
    virus->health = new_health;
}

bool virus_is_super_virus(virus_t *virus)
{
    return virus->is_super_virus;
}

rgb_color_t virus_get_color(int health)
{
    if (health % 5 == 0)
    {
        return BLACK;
    }
    return color_rainbow(health * VIRUS_COLOR_MULTIPLIER);
}

void virus_set_path_nodes(body_t *virus_body, int path_nodes)
{
    ((virus_t *)get_global_secondary_info(virus_body))->path_nodes = path_nodes;
}

int virus_get_path_nodes(virus_t *virus)
{
    return virus->path_nodes;
}
