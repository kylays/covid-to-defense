#include "tower.h"
#include "math.h"
#include "sound.h"

/////////////////////////// CONSTS & CONFIGS ////////////////////////////////

const double TOWER_MASS = (double)INFINITY;
const double TOWER_RADIUS = 20.0;
const size_t TOWER_CIRCLE_POINTS = 10;
const size_t TOWER_COLOR_MULTIPLIER = 3;
const size_t TOWER_RANGE_NUM_POINTS = 50;
const rgb_color_t RANGE_COLOR = {.r = 1, .g = 0, .b = 0};
const int TOWER_INITIAL_TIME = 0;
const int VIRUS_DAMAGE_MONEY_MULTIPLIER = 5;
const double MIN_BULLET_SPEED = 1000.0; // min speed where bullets can still reasonably hit first virus

const double AIRPLANE_G = 2500;
const double AIRPLANE_FLIGHT_RADIUS = 150;
const double AIRPLANE_PATH_THETA_CONST = M_PI / 125;
const int AIRPLANE_BOMB_INT_MAX = 2147483647;

const double BOOMERANG_SPRING_CONSTANT_K = 25.0;
const double BOOMERANG_DRAG = 0.25;

const double HIGH_ALIVE_TIME = 100;
const double NORMAL_ALIVE_TIME = 50;
const double LOW_ALIVE_TIME = 25;

const vector_t BOMB_OFFSET = {.x = 10, .y = 10};

tower_t tower_make_struct(int price, double range, int upgrade_level, bullet_t bullet_info,
                          int time_counter, int shoot_interval, int bonus,
                          tower_id_t id, char *description)
{
    return (tower_t){.price = price, .range = range, .upgrade_level = upgrade_level,
                        .bullet_info = bullet_info, .time_counter = time_counter,
                        .shoot_interval = shoot_interval, .bonus = bonus, .id = id,
                        .bomb_time_counter = 0, .bomb_interval = 0, .flight_center = VEC_ZERO,
                        .description = description};
}

tower_t tower_make_airplane_struct(int price, double range, int upgrade_level, bullet_t bullet_info,
                                   int time_counter, int shoot_interval, int bonus, tower_id_t id,
                                   int bomb_time_counter, int bomb_interval, char *description)
{
    tower_t t = tower_make_struct(price, range, upgrade_level, bullet_info, time_counter, shoot_interval, bonus, id, description);
    t.bomb_time_counter = bomb_time_counter;
    t.bomb_interval = bomb_interval;
    return t;
}

void setup_tower_configs()
{
    // ========== DART TOWER ==========
    DART_TOWER_BULLET = bullet_make_struct(MIN_BULLET_SPEED, 1, 1.0, 1, NORMAL_ALIVE_TIME);
    DART_TOWER = tower_make_struct(50, 100.0, UPGRADE_LEVEL_1, DART_TOWER_BULLET,
                                   TOWER_INITIAL_TIME, 75, 0, DART_TOWER_ID,
                                   "Dart Tower: basic bullets");
    DART_TOWER_BULLET_2 = bullet_make_struct(MIN_BULLET_SPEED + 150.0, 1, 1.0, 1, NORMAL_ALIVE_TIME);
    DART_TOWER_2 = tower_make_struct(30, 100.0, UPGRADE_LEVEL_2, DART_TOWER_BULLET,
                                     TOWER_INITIAL_TIME, 50, 0, DART_TOWER_ID,
                                     "Dart Tower: increase speed");
    DART_TOWER_BULLET_3 = bullet_make_struct(MIN_BULLET_SPEED + 150.0, 1, 1.0, 1, NORMAL_ALIVE_TIME);
    DART_TOWER_3 = tower_make_struct(40, 200.0, UPGRADE_LEVEL_3, DART_TOWER_BULLET,
                                     TOWER_INITIAL_TIME, 50, 0, DART_TOWER_ID,
                                     "Dart Tower: increase range");
    DART_TOWER_BULLET_4 = bullet_make_struct(MIN_BULLET_SPEED + 250.0, 2, 1.0, 1, NORMAL_ALIVE_TIME);
    DART_TOWER_4 = tower_make_struct(75, 200.0, UPGRADE_LEVEL_4, DART_TOWER_BULLET,
                                     TOWER_INITIAL_TIME, 25, 0, DART_TOWER_ID,
                                     "Dart Tower: increase speed & frequency");

    // ========== TACK SHOOTER ==========
    TACK_SHOOTER_BULLET = bullet_make_struct(MIN_BULLET_SPEED, 1, 1.0, 5, 7);
    TACK_SHOOTER = tower_make_struct(75, 75.0, UPGRADE_LEVEL_1, TACK_SHOOTER_BULLET,
                                     TOWER_INITIAL_TIME, 50, 0, TACK_SHOOTER_ID,
                                     "Tack Shooter*: 5 direction bullets");
    TACK_SHOOTER_BULLET_2 = bullet_make_struct(MIN_BULLET_SPEED, 1, 1.0, 5, 7);
    TACK_SHOOTER_2 = tower_make_struct(75, 75.0, UPGRADE_LEVEL_2, TACK_SHOOTER_BULLET,
                                       TOWER_INITIAL_TIME, 30, 0, TACK_SHOOTER_ID,
                                       "Tack Shooter*: increase frequency");
    TACK_SHOOTER_BULLET_3 = bullet_make_struct(MIN_BULLET_SPEED + 100.0, 1, 1.0, 15, 7);
    TACK_SHOOTER_3 = tower_make_struct(125, 75.0, UPGRADE_LEVEL_3, TACK_SHOOTER_BULLET,
                                       TOWER_INITIAL_TIME, 30, 0, TACK_SHOOTER_ID,
                                       "Tack Shooter*: increase radius & directions");
    TACK_SHOOTER_BULLET_4 = bullet_make_struct(MIN_BULLET_SPEED + 250.0, 2, 1.0, 20, 7);
    TACK_SHOOTER_4 = tower_make_struct(200, 75.0, UPGRADE_LEVEL_4, TACK_SHOOTER_BULLET,
                                       TOWER_INITIAL_TIME, 20, 0, TACK_SHOOTER_ID,
                                       "Tack Shooter*: increase frequency & damage");

    // ========== BOMB SHOOTER ==========
    BOMB_SHOOTER_BULLET = bullet_make_struct(MIN_BULLET_SPEED, 3, 1.0, 5, LOW_ALIVE_TIME);
    BOMB_SHOOTER = tower_make_struct(150, 125.0, UPGRADE_LEVEL_1, BOMB_SHOOTER_BULLET,
                                     TOWER_INITIAL_TIME, 100, 0, BOMB_SHOOTER_ID,
                                     "Bomb Shooter: high damage bullets");
    BOMB_SHOOTER_BULLET_2 = bullet_make_struct(MIN_BULLET_SPEED, 3, 1.0, 5, LOW_ALIVE_TIME);
    BOMB_SHOOTER_2 = tower_make_struct(100, 175.0, UPGRADE_LEVEL_2, BOMB_SHOOTER_BULLET,
                                       TOWER_INITIAL_TIME, 75, 0, BOMB_SHOOTER_ID,
                                       "Bomb Shooter: increase damage");
    BOMB_SHOOTER_BULLET_3 = bullet_make_struct(MIN_BULLET_SPEED + 250.0, 5, 1.0, 5, LOW_ALIVE_TIME);
    BOMB_SHOOTER_3 = tower_make_struct(250, 175.0, UPGRADE_LEVEL_3, BOMB_SHOOTER_BULLET,
                                       TOWER_INITIAL_TIME, 50, 0, BOMB_SHOOTER_ID,
                                       "Bomb Shooter: increase radius");
    BOMB_SHOOTER_BULLET_4 = bullet_make_struct(MIN_BULLET_SPEED + 250.0, 6, 1.0, 5, LOW_ALIVE_TIME);
    BOMB_SHOOTER_4 = tower_make_struct(500, 175.0, UPGRADE_LEVEL_4, BOMB_SHOOTER_BULLET,
                                       TOWER_INITIAL_TIME, 20, 0, BOMB_SHOOTER_ID,
                                       "Bomb Shooter: increase damage & radius");
    BOMB_SHOOTER_BOMB = tower_make_struct(0, 0, UPGRADE_ID, BOMB_SHOOTER_BULLET,
                                          TOWER_INITIAL_TIME, 0, 0, BOMB_SHOOTER_ID, NULL);

    // ========== SUPER TOWER ==========
    SUPER_TOWER_BULLET = bullet_make_struct(MIN_BULLET_SPEED * 2, 2, 1.0, 1, NORMAL_ALIVE_TIME);
    SUPER_TOWER = tower_make_struct(1000, 200.0, UPGRADE_LEVEL_1, SUPER_TOWER_BULLET,
                                    TOWER_INITIAL_TIME, 5, 0, SUPER_TOWER_ID,
                                    "Super Tower*: high damage, frequency, range");
    SUPER_TOWER_BULLET_2 = bullet_make_struct(MIN_BULLET_SPEED * 2 + 250.0, 2, 1.0, 1, NORMAL_ALIVE_TIME);
    SUPER_TOWER_2 = tower_make_struct(1000, 250.0, UPGRADE_LEVEL_2, SUPER_TOWER_BULLET,
                                      TOWER_INITIAL_TIME, 5, 0, SUPER_TOWER_ID,
                                      "Super Tower*: increase radius");
    SUPER_TOWER_BULLET_3 = bullet_make_struct(MIN_BULLET_SPEED * 3, 3, 1.0, 1, NORMAL_ALIVE_TIME);
    SUPER_TOWER_3 = tower_make_struct(2000, 300.0, UPGRADE_LEVEL_3, SUPER_TOWER_BULLET,
                                      TOWER_INITIAL_TIME, 3, 0, SUPER_TOWER_ID,
                                      "Super Tower*: increase frequency");
    SUPER_TOWER_BULLET_4 = bullet_make_struct(MIN_BULLET_SPEED * 3 + 250.0, 4, 1.0, 1, NORMAL_ALIVE_TIME);
    SUPER_TOWER_4 = tower_make_struct(2750, 500.0, UPGRADE_LEVEL_4, SUPER_TOWER_BULLET,
                                      TOWER_INITIAL_TIME, 3, 0, SUPER_TOWER_ID,
                                      "Super Tower*: increase damage");

    // ========== AIRPLANE ==========
    AIRPLANE_BULLET = bullet_make_struct(MIN_BULLET_SPEED / 2, 1, 1.0, 1, HIGH_ALIVE_TIME);
    AIRPLANE = tower_make_airplane_struct(800, 350.0, UPGRADE_LEVEL_1, AIRPLANE_BULLET,
                                          TOWER_INITIAL_TIME, 15, 0, AIRPLANE_ID,
                                          TOWER_INITIAL_TIME, AIRPLANE_BOMB_INT_MAX,
                                          "Airplane*: pursuit bullets");
    AIRPLANE_BULLET_2 = bullet_make_struct(MIN_BULLET_SPEED / 2, 1, 1.0, 1, HIGH_ALIVE_TIME);
    AIRPLANE_2 = tower_make_airplane_struct(1000, 350.0, UPGRADE_LEVEL_2, AIRPLANE_BULLET,
                                            TOWER_INITIAL_TIME, 15, 0, AIRPLANE_ID,
                                            TOWER_INITIAL_TIME, AIRPLANE_BOMB_INT_MAX,
                                            "Airplane*: increase frequency");
    AIRPLANE_BULLET_3 = bullet_make_struct(MIN_BULLET_SPEED / 2, 1, 1.0, 1, HIGH_ALIVE_TIME);
    AIRPLANE_3 = tower_make_airplane_struct(1250, 350.0, UPGRADE_LEVEL_3, AIRPLANE_BULLET,
                                            TOWER_INITIAL_TIME, AIRPLANE_BOMB_INT_MAX, 0, AIRPLANE_ID, TOWER_INITIAL_TIME,
                                            40, "Airplane*: increase damage");
    AIRPLANE_BULLET_4 = bullet_make_struct(MIN_BULLET_SPEED / 2, 1, 1.0, 1, HIGH_ALIVE_TIME);
    AIRPLANE_4 = tower_make_airplane_struct(1250, 350.0, UPGRADE_LEVEL_4, AIRPLANE_BULLET,
                                            TOWER_INITIAL_TIME, 10, 0, AIRPLANE_ID, TOWER_INITIAL_TIME,
                                            25, "Airplane*: drops bombs");
    AIRPLANE_BOMB_BULLET = bullet_make_struct(MIN_BULLET_SPEED, 1, 1.0, 5, LOW_ALIVE_TIME);
    AIRPLANE_BOMB = tower_make_struct(0, 75.0, UPGRADE_ID, TACK_SHOOTER_BULLET, TOWER_INITIAL_TIME,
                                      AIRPLANE_BOMB_INT_MAX, 0, AIRPLANE_ID, NULL);

    // ========== GLUE TOWER ==========
    GLUE_TOWER_BULLET = bullet_make_struct(MIN_BULLET_SPEED + 200.0, 0, 1000, 1, NORMAL_ALIVE_TIME);
    GLUE_TOWER = tower_make_struct(250, 150.0, UPGRADE_LEVEL_1, GLUE_TOWER_BULLET,
                                   TOWER_INITIAL_TIME, 100, 0, GLUE_TOWER_ID,
                                   "Glue Tower: slows down viruses");
    GLUE_TOWER_BULLET_2 = bullet_make_struct(MIN_BULLET_SPEED + 250.0, 0, 2000, 1, NORMAL_ALIVE_TIME);
    GLUE_TOWER_2 = tower_make_struct(175, 150.0, UPGRADE_LEVEL_2, GLUE_TOWER_BULLET,
                                     TOWER_INITIAL_TIME, 75, 0, GLUE_TOWER_ID,
                                     "Glue Tower: increase slow down");
    GLUE_TOWER_BULLET_3 = bullet_make_struct(MIN_BULLET_SPEED + 300.0, 1, 3000, 1, NORMAL_ALIVE_TIME);
    GLUE_TOWER_3 = tower_make_struct(300, 225.0, UPGRADE_LEVEL_3, GLUE_TOWER_BULLET,
                                     TOWER_INITIAL_TIME, 50, 0, GLUE_TOWER_ID,
                                     "Glue Tower: increase range");
    GLUE_TOWER_BULLET_4 = bullet_make_struct(MIN_BULLET_SPEED + 400.0, 1, 4000, 1, NORMAL_ALIVE_TIME);
    GLUE_TOWER_4 = tower_make_struct(500, 225.0, UPGRADE_LEVEL_4, GLUE_TOWER_BULLET,
                                     TOWER_INITIAL_TIME, 25, 0, GLUE_TOWER_ID,
                                     "Glue Tower: increase slow down");

    // ========== BOOMERANG ==========
    BOOMERANG_BULLET = bullet_make_struct(MIN_BULLET_SPEED, 1, 0, 1, HIGH_ALIVE_TIME);
    BOOMERANG = tower_make_struct(250, 150.0, UPGRADE_LEVEL_1, BOOMERANG_BULLET,
                                  TOWER_INITIAL_TIME, 100, 0, BOOMERANG_ID,
                                  "Boomerang*: boomerang bullets");
    BOOMERANG_BULLET_2 = bullet_make_struct(MIN_BULLET_SPEED, 1, 0, 1, HIGH_ALIVE_TIME);
    BOOMERANG_2 = tower_make_struct(175, 200.0, UPGRADE_LEVEL_2, BOOMERANG_BULLET,
                                    TOWER_INITIAL_TIME, 50, 0, BOOMERANG_ID,
                                    "Boomerang*: increase frequency & range");
    BOOMERANG_BULLET_3 = bullet_make_struct(MIN_BULLET_SPEED + 250.0, 2, 0, 1, HIGH_ALIVE_TIME);
    BOOMERANG_3 = tower_make_struct(300, 200.0, UPGRADE_LEVEL_3, BOOMERANG_BULLET,
                                    TOWER_INITIAL_TIME, 50, 0, BOOMERANG_ID,
                                    "Boomerang*: increase speed ");
    BOOMERANG_BULLET_4 = bullet_make_struct(MIN_BULLET_SPEED + 500.0, 2, 0, 1, HIGH_ALIVE_TIME);
    BOOMERANG_4 = tower_make_struct(500, 200.0, UPGRADE_LEVEL_4, BOOMERANG_BULLET,
                                    TOWER_INITIAL_TIME, 25, 0, BOOMERANG_ID,
                                    "Boomerang*: increase speed & frequency");

    // ========== BANK ==========
    NULL_BULLET = bullet_make_struct(0, 0, 0, 0, 0);
    BANK = tower_make_struct(400, 0, UPGRADE_LEVEL_1, NULL_BULLET, 0, 500, 25,
                             BANK_ID, "Bank: get $25 per 500 ticks");
    BANK_2 = tower_make_struct(500, 0, UPGRADE_LEVEL_2, NULL_BULLET, 0, 500, 75,
                               BANK_ID, "Bank: get $75");
    BANK_3 = tower_make_struct(600, 0, UPGRADE_LEVEL_3, NULL_BULLET, 0, 300, 75,
                               BANK_ID, "Bank: increase frequency");
    BANK_4 = tower_make_struct(750, 0, UPGRADE_LEVEL_4, NULL_BULLET, 0, 200, 150,
                               BANK_ID, "Bank: get $150, increase frequency");
}

//////////////////////////////////////////////////////////////////////////////////////////////

// PRIVATE FUNCTION DECLARATIONS
body_t *tower_get_front_virus(scene_t *scene, vector_t tower_position, double radius);
vector_t tower_aim(body_t *tower_body, scene_t *scene);
void tower_shoot_standard(body_t *tower_body, scene_t *scene, game_state_t *game_state, vector_t direction);
void tower_tack_shoot(body_t *tower_body, scene_t *scene, game_state_t *game_state, bool alwaysActivate);
void virus_boomerang_collision(body_t *virus_body, body_t *bullet, vector_t axis, game_state_t *game_state);
void virus_glue_collision(body_t *virus, body_t *bullet, vector_t axis, game_state_t *game_state);
void virus_bomb_collision(body_t *virus_body, body_t *bullet, vector_t axis, game_state_t *game_state);
void virus_bullet_collision(body_t *virus_body, body_t *bullet, vector_t axis, game_state_t *game_state);
void virus_damage(body_t *virus_body, int damage, game_state_t *game_state);
void tower_airplane_bomb(body_t *tower_body, scene_t *scene, game_state_t *game_state);
void tower_airplane_update(body_t *tower_body, scene_t *scene);

void virus_damage(body_t *virus_body, int damage, game_state_t *game_state)
{
    virus_t *info = get_global_secondary_info(virus_body);
    int health = virus_get_health(info);

    sound_t *pop_sound = game_state->pop_sound;
    sound_play(pop_sound);

    if (health < 0)
    {
        health = 0;
    }
    int points = (int)fmin(damage, health);
    assert(health >= 0);

    game_state->score += points;
    game_state->money += points * VIRUS_DAMAGE_MONEY_MULTIPLIER;

    virus_set_health(info, health - damage);

    if (virus_get_health(info) <= 0 && !body_is_removed(virus_body))
    {
        if (virus_is_super_virus(info))
        {
            for (int i = 0; i < SUPER_VIRUS_SPAWN_COUNT; i++)
            {
                // spawn viruses from super virus
                body_t *new_virus_body = create_virus(game_state->scene, SUPER_VIRUS_SPAWN_HEALTH,
                                                      vec_multiply(SUPER_VIRUS_SPEED_MULTIPLIER + i * 0.05,
                                                                   body_get_velocity(virus_body)),
                                                      body_get_centroid(virus_body), game_state, false);
                virus_set_path_nodes(new_virus_body, virus_get_path_nodes(info));
            }
        }

        body_remove(virus_body);
    }
    else
    {
        body_set_color(virus_body, virus_get_color(virus_get_health(info)));
    }
}

void virus_bullet_collision(body_t *virus_body, body_t *bullet, vector_t axis, game_state_t *game_state)
{
    virus_damage(virus_body, ((bullet_t *)get_global_secondary_info(bullet))->damage, game_state);
    body_remove(bullet);
}

void virus_glue_collision(body_t *virus, body_t *bullet, vector_t axis, game_state_t *game_state)
{
    vector_t force = vec_negate(vec_multiply(((bullet_t *)get_global_secondary_info(bullet))->glue_slowdown * body_get_mass(virus),
                                             vec_unit(body_get_velocity(virus))));
    body_add_force(virus, force);
    virus_damage(virus, ((bullet_t *)get_global_secondary_info(bullet))->damage, game_state);
    body_remove(bullet);
}

void virus_boomerang_collision(body_t *virus_body, body_t *bullet, vector_t axis, game_state_t *game_state)
{
    virus_damage(virus_body, ((bullet_t *)get_global_secondary_info(bullet))->damage, game_state);
}

void bomb_effect(vector_t position, scene_t *scene, game_state_t *game_state, tower_t bomb_type)
{
    tower_t *t = malloc(sizeof(tower_t));
    *t = bomb_type;
    body_t *bomb = body_init_with_secondary_info(polygon_make_circle(position, 3, 3),
                                                 TOWER_MASS, tower_get_color(t->id),
                                                 t, free, TOWER_TYPE);
    tower_tack_shoot(bomb, scene, game_state, true);
    body_free(bomb);
}

void virus_bomb_collision(body_t *virus_body, body_t *bullet, vector_t axis, game_state_t *game_state)
{
    scene_t *scene = game_state->scene;

    // Damaging all viruses within a radius = bomb explosion
    int radius = 20;
    for (size_t i = 0; i < scene_bodies(scene); i++)
    {
        body_t *body = scene_get_body(scene, i);
        if (get_global_type(body) == VIRUS_TYPE && vec_magnitude(vec_subtract(body_get_centroid(body), body_get_centroid(virus_body))) <= radius)
        {
            virus_damage(body, ((bullet_t *)get_global_secondary_info(bullet))->damage, game_state);
        }
    }

    body_remove(bullet);
    create_explosion_effect_bullet(scene, body_get_centroid(virus_body));
}

void tower_tick(body_t *tower_body, scene_t *scene, game_state_t *game_state)
{
    tower_t *tower = get_global_secondary_info(tower_body);
    int tower_id = tower_get_type(tower_body);
    tower->time_counter++;

    // updating the tower airplane path
    if (tower_id == AIRPLANE_ID)
    {
        tower_airplane_update(tower_body, scene);
    }

    if (tower_id == BANK_ID)
    {
        if (tower->time_counter >= tower->shoot_interval && game_state->virus_count != 0)
        {
            printf("bonus money %d\n", tower->bonus);
            game_state->money += tower->bonus;
            tower->time_counter = TOWER_INITIAL_TIME;
        }
    }

    if (tower->time_counter >= tower->shoot_interval)
    {
        if (tower_get_type(tower_body) == TACK_SHOOTER_ID)
        {
            tower_tack_shoot(tower_body, scene, game_state, false);
        }
        else
        {
            vector_t direction = tower_aim(tower_body, scene);
            tower_shoot_standard(tower_body, scene, game_state, direction);
        }
        tower->time_counter = TOWER_INITIAL_TIME;
    }

    if (tower->id == AIRPLANE_ID && (tower->bomb_interval != AIRPLANE_BOMB_INT_MAX) &&
        (tower->bomb_time_counter >= tower->bomb_interval))
    {
        tower->bomb_time_counter = TOWER_INITIAL_TIME;
        tower_airplane_bomb(tower_body, scene, game_state);
    }
    else
    {
        tower->bomb_time_counter++;
    }
}

void tower_airplane_update(body_t *tower_body, scene_t *scene)
{
    // 1. get center of path circle
    tower_t *tower_info = get_global_secondary_info(tower_body);
    vector_t airplane_pos = body_get_centroid(tower_body);
    vector_t path_center = tower_info->flight_center;

    // 2. get theta position of the tower_body on the path circle
    double dx = airplane_pos.x - path_center.x;
    double dy = airplane_pos.y - path_center.y;
    double theta = atan2(dy, dx) + AIRPLANE_PATH_THETA_CONST;

    // 3. update its position on the circle
    airplane_pos = (vector_t){.x = cos(theta) * AIRPLANE_FLIGHT_RADIUS,
                              .y = sin(theta) * AIRPLANE_FLIGHT_RADIUS};
    airplane_pos = vec_add(airplane_pos, path_center);
    body_set_centroid(tower_body, airplane_pos);
}

void tower_airplane_bomb(body_t *tower_body, scene_t *scene, game_state_t *game_state)
{
    bomb_effect(body_get_centroid(tower_body), scene, game_state, AIRPLANE_BOMB);
}

body_t *tower_get_front_virus(scene_t *scene, vector_t tower_position, double radius)
{
    double max = 0.0;
    body_t *front_virus; // reference variable to the farthest along virus
    for (size_t i = 0; i < scene_bodies(scene); i++)
    {
        body_t *body = scene_get_body(scene, i);
        if (get_global_type(body) == VIRUS_TYPE)
        {
            if (vec_magnitude(vec_subtract(body_get_centroid(body), tower_position)) <= radius)
            {
                virus_t *virus = get_global_secondary_info(body);
                double dist = virus_get_distance_travelled(virus);
                if (dist > max)
                {
                    max = dist;
                    front_virus = body;
                }
            }
        }
    }
    if (max == 0.0)
    {
        return NULL;
    }
    return front_virus;
}

vector_t tower_aim(body_t *tower_body, scene_t *scene)
{
    tower_t *tower = get_global_secondary_info(tower_body);
    body_t *virus_body = tower_get_front_virus(scene, body_get_centroid(tower_body), tower->range);
    vector_t direction = VEC_ZERO;
    if (virus_body != NULL)
    {
        virus_t *virus_info = get_global_secondary_info(virus_body);
        if (virus_info->health % 5 == 0 && tower->id % 3 != 1)
        {
            return direction;
        }
        direction.x = vec_subtract(body_get_centroid(virus_body), body_get_centroid(tower_body)).x;
        direction.y = vec_subtract(body_get_centroid(virus_body), body_get_centroid(tower_body)).y;
        direction = vec_unit(direction);
    }
    return direction;
}

void tower_shoot_standard(body_t *tower_body, scene_t *scene, game_state_t *game_state, vector_t direction)
{
    tower_t *tower_info = get_global_secondary_info(tower_body);
    bullet_t bullet_info = tower_info->bullet_info;

    if (direction.x == 0 && direction.y == 0)
    {
        return;
    }

    // Create bullet at tower position
    body_t *bullet = create_basic_bullet(scene, body_get_centroid(tower_body),
                                         body_get_color(tower_body), bullet_info);
    body_set_centroid(bullet, body_get_centroid(tower_body));

    // set bullet velocity in direction of front virus
    double bullet_speed = bullet_info.speed;
    body_set_velocity(bullet, vec_multiply(bullet_speed, direction));

    // if pursuit bullet, add gravity
    body_t *virus_body = tower_get_front_virus(scene, body_get_centroid(tower_body), tower_info->range);
    if (tower_info->id == AIRPLANE_ID && virus_body)
    {
        create_one_sided_gravity(scene, AIRPLANE_G, bullet, virus_body);
    }

    // Create destructive force between bullet and all viruses
    for (size_t i = 0; i < scene_bodies(scene); i++)
    {
        body_t *curr_body = scene_get_body(scene, i);
        if (get_global_type(curr_body) == VIRUS_TYPE)
        {
            if (tower_info->id == BOMB_SHOOTER_ID)
            {
                create_collision(scene, curr_body, bullet,
                                 (collision_handler_t)virus_bomb_collision, game_state, NULL);
            }
            else if (tower_info->id == GLUE_TOWER_ID)
            {
                create_collision(scene, curr_body, bullet,
                                 (collision_handler_t)virus_glue_collision, game_state, NULL);
            }
            if (tower_info->id == BOOMERANG_ID)
            {
                create_spring(scene, BOOMERANG_SPRING_CONSTANT_K, bullet, tower_body);
                create_drag(scene, BOOMERANG_DRAG, bullet);
                create_collision(scene, curr_body, bullet,
                                 (collision_handler_t)virus_boomerang_collision, game_state, NULL);
            }
            else
            {
                create_collision(scene, curr_body, bullet,
                                 (collision_handler_t)virus_bullet_collision, game_state, NULL);
            }
        }
        else if (get_global_type(curr_body) == WALL_TYPE)
        {
            create_one_sided_destructive_collision(scene, bullet, curr_body);
        }
    }
}

void tower_tack_shoot(body_t *tower_body, scene_t *scene, game_state_t *game_state, bool alwaysActivate)
{
    tower_t *tower_info = get_global_secondary_info(tower_body);
    for (size_t i = 0; i < scene_bodies(scene); i++)
    {
        body_t *body = scene_get_body(scene, i);
        if (get_global_type(body) == VIRUS_TYPE || alwaysActivate)
        {
            double distance = vec_magnitude(vec_subtract(body_get_centroid(body),
                                                         body_get_centroid(tower_body)));
            if ((distance <= tower_info->range) || alwaysActivate)
            {
                int n = tower_info->bullet_info.num_tack_directions;
                for (double i = 0; i < 2 * M_PI; i += 2 * M_PI / n)
                {
                    vector_t direction = {.x = cos(i), .y = sin(i)};
                    tower_shoot_standard(tower_body, scene, game_state, direction);
                }
                return;
            }
        }
    }
}

//////////////////////////////////////////// PUBLIC TOWER FUNCTIONS //////////////////////////////////////////////////

body_t *create_tower(scene_t *scene, tower_t tower_info, vector_t position)
{
    if (tower_info.id == AIRPLANE_ID)
    {
        tower_info.flight_center = position;
    }
    tower_t *tower = malloc(sizeof(tower_t));
    assert(tower != NULL);
    *tower = tower_info;
    list_t *shape = polygon_make_circle(position, TOWER_RADIUS, TOWER_CIRCLE_POINTS);
    body_t *body = body_init_with_secondary_info(shape, TOWER_MASS, tower_get_color(tower->id),
                                                 tower, (free_func_t)tower_free, TOWER_TYPE);
    scene_add_body(scene, body);
    return body;
}

void tower_free(tower_t *tower_info)
{
    free(tower_info);
}

void tower_display_range(body_t *tower_body, scene_t *scene)
{
    vector_t center = body_get_centroid(tower_body);
    tower_t *tower_info = get_global_secondary_info(tower_body);
    list_t *donut = polygon_make_circle(center, tower_info->range, TOWER_RANGE_NUM_POINTS);
    double radius = tower_info->range - 3;

    for (double i = 2 * M_PI; i > 0; i -= 2 * M_PI / TOWER_RANGE_NUM_POINTS)
    {
        double x = radius * cos(i);
        double y = radius * sin(i);

        vector_t *point = malloc(sizeof(vector_t));
        point->x = center.x + x;
        point->y = center.y + y;

        list_add(donut, point);
    }

    body_t *body = body_init_with_secondary_info(donut, INFINITY, RANGE_COLOR,
                                                 NULL, NULL, TOWER_RANGE_DISPLAY);
    scene_add_body(scene, body);
}

void tower_undisplay_range(scene_t *scene)
{
    for (int i = 0; i < scene_bodies(scene); i++)
    {
        body_t *body = scene_get_body(scene, i);
        if (get_global_type(body) == TOWER_RANGE_DISPLAY)
        {
            scene_remove_body(scene, i);
        }
    }
}

// ACCESSORS

double tower_get_radius()
{
    return TOWER_RADIUS;
}

tower_id_t tower_get_type(body_t *tower_body)
{
    tower_t *tower = get_global_secondary_info(tower_body);
    return tower->id;
}

rgb_color_t tower_get_color(tower_id_t id)
{
    return color_rainbow(id * TOWER_COLOR_MULTIPLIER);
}

double tower_get_airplane_flight_radius()
{
    return AIRPLANE_FLIGHT_RADIUS;
}

char *tower_get_description_from_id(tower_id_t id)
{
    return tower_get_from_id(id).description;
}

tower_t tower_get_from_id(tower_id_t tower_id)
{
    // called when placing tower after purchasing item
    return tower_get_from_id_and_level(tower_id, UPGRADE_LEVEL_1);
}

tower_t tower_get_from_id_and_level(int tower_id, int upgrade_level)
{
    if (upgrade_level == UPGRADE_LEVEL_1)
    {
        switch (tower_id)
        {
        case DART_TOWER_ID:
            return DART_TOWER;
        case TACK_SHOOTER_ID:
            return TACK_SHOOTER;
        case BOMB_SHOOTER_ID:
            return BOMB_SHOOTER;
        case SUPER_TOWER_ID:
            return SUPER_TOWER;
        case BANK_ID:
            return BANK;
        case AIRPLANE_ID:
            return AIRPLANE;
        case GLUE_TOWER_ID:
            return GLUE_TOWER;
        case BOOMERANG_ID:
            return BOOMERANG;
        }
    }
    else if (upgrade_level == UPGRADE_LEVEL_2)
    {
        switch (tower_id)
        {
        case DART_TOWER_ID:
            return DART_TOWER_2;
        case TACK_SHOOTER_ID:
            return TACK_SHOOTER_2;
        case BOMB_SHOOTER_ID:
            return BOMB_SHOOTER_2;
        case SUPER_TOWER_ID:
            return SUPER_TOWER_2;
        case BANK_ID:
            return BANK_2;
        case AIRPLANE_ID:
            return AIRPLANE_2;
        case GLUE_TOWER_ID:
            return GLUE_TOWER_2;
        case BOOMERANG_ID:
            return BOOMERANG_2;
        }
    }
    else if (upgrade_level == UPGRADE_LEVEL_3)
    {
        switch (tower_id)
        {
        case DART_TOWER_ID:
            return DART_TOWER_3;
        case TACK_SHOOTER_ID:
            return TACK_SHOOTER_3;
        case BOMB_SHOOTER_ID:
            return BOMB_SHOOTER_3;
        case SUPER_TOWER_ID:
            return SUPER_TOWER_3;
        case BANK_ID:
            return BANK_3;
        case AIRPLANE_ID:
            return AIRPLANE_3;
        case GLUE_TOWER_ID:
            return GLUE_TOWER_3;
        case BOOMERANG_ID:
            return BOOMERANG_3;
        }
    }
    else if (upgrade_level == UPGRADE_LEVEL_4)
    {
        switch (tower_id)
        {
        case DART_TOWER_ID:
            return DART_TOWER_4;
        case TACK_SHOOTER_ID:
            return TACK_SHOOTER_4;
        case BOMB_SHOOTER_ID:
            return BOMB_SHOOTER_4;
        case SUPER_TOWER_ID:
            return SUPER_TOWER_4;
        case BANK_ID:
            return BANK_4;
        case AIRPLANE_ID:
            return AIRPLANE_4;
        case GLUE_TOWER_ID:
            return GLUE_TOWER_4;
        case BOOMERANG_ID:
            return BOOMERANG_4;
        }
    }
    return DART_TOWER;
}
