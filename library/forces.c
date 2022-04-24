#include <math.h>
#include "forces.h"
#include "collision.h"

const double DISTANCE_THRESHOLD = 5.0;

typedef struct force_aux
{
    scene_t *scene;
    double constant;
    list_t *bodies;
} force_aux_t;

void force_aux_free(force_aux_t *aux)
{
    list_free(aux->bodies);
    free(aux);
}

void gravity_force_creator(force_aux_t *aux)
{
    assert(list_size(aux->bodies) == 2);
    body_t *body1 = list_get(aux->bodies, 0);
    body_t *body2 = list_get(aux->bodies, 1);
    double mass1 = body_get_mass(body1);
    double mass2 = body_get_mass(body2);
    double distance = vec_distance(body_get_centroid(body1), body_get_centroid(body2));
    vector_t r = vec_subtract(body_get_centroid(body2), body_get_centroid(body1));

    if (distance > DISTANCE_THRESHOLD)
    {
        vector_t force1 = vec_multiply((aux->constant * (mass1 * mass2) / pow(distance, 3.0)), r);
        vector_t force2 = vec_negate(force1);
        body_add_force(body1, force1);
        body_add_force(body2, force2);
    }
}

void create_newtonian_gravity(scene_t *scene, double G, body_t *body1, body_t *body2)
{
    force_aux_t *grav_aux = malloc(sizeof(force_aux_t));
    assert(grav_aux != NULL);
    list_t *bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);
    *grav_aux = (force_aux_t){.scene = scene, .constant = G, .bodies = bodies};
    scene_add_bodies_force_creator(scene, (force_creator_t)gravity_force_creator,
                                   grav_aux, bodies, (free_func_t)force_aux_free);
}

void one_sided_gravity_force_creator(force_aux_t *aux)
{
    assert(list_size(aux->bodies) == 2);
    body_t *body1 = list_get(aux->bodies, 0);
    body_t *body2 = list_get(aux->bodies, 1);
    double mass1 = body_get_mass(body1);
    double mass2 = body_get_mass(body2);
    double distance = vec_distance(body_get_centroid(body1), body_get_centroid(body2));
    vector_t r = vec_subtract(body_get_centroid(body2), body_get_centroid(body1));

    if (distance > DISTANCE_THRESHOLD)
    {
        vector_t force1 = vec_multiply((aux->constant * (mass1 * mass2) / pow(distance, 3.0)), r);
        body_add_force(body1, force1);
    }
}

void create_one_sided_gravity(scene_t *scene, double G, body_t *body1, body_t *body2)
{
    force_aux_t *grav_aux = malloc(sizeof(force_aux_t));
    assert(grav_aux != NULL);
    list_t *bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);
    *grav_aux = (force_aux_t){.scene = scene, .constant = G, .bodies = bodies};
    scene_add_bodies_force_creator(scene, (force_creator_t)one_sided_gravity_force_creator,
                                   grav_aux, bodies, (free_func_t)force_aux_free);
}

void spring_force_creator(force_aux_t *aux)
{
    assert(list_size(aux->bodies) == 2);
    body_t *body1 = list_get(aux->bodies, 0);
    body_t *body2 = list_get(aux->bodies, 1);
    vector_t force1 = vec_multiply(aux->constant, vec_subtract(body_get_centroid(body2),
                                                               body_get_centroid(body1)));
    vector_t force2 = vec_negate(force1);
    body_add_force(body1, force1);
    body_add_force(body2, force2);
}

void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2)
{
    force_aux_t *spring_aux = malloc(sizeof(force_aux_t));
    assert(spring_aux != NULL);
    list_t *bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);
    *spring_aux = (force_aux_t){.scene = scene, .constant = k, .bodies = bodies};
    scene_add_bodies_force_creator(scene, (force_creator_t)spring_force_creator,
                                   spring_aux, bodies, (free_func_t)force_aux_free);
}

void drag_force_creator(force_aux_t *aux)
{
    assert(list_size(aux->bodies) == 1);
    body_t *body = list_get(aux->bodies, 0);
    vector_t force = vec_negate(vec_multiply(aux->constant, body_get_velocity(body)));
    body_add_force(body, force);
}

void create_drag(scene_t *scene, double gamma, body_t *body)
{
    force_aux_t *drag_aux = malloc(sizeof(force_aux_t));
    assert(drag_aux != NULL);
    list_t *bodies = list_init(1, NULL);
    list_add(bodies, body);
    *drag_aux = (force_aux_t){.scene = scene, .constant = gamma, .bodies = bodies};
    scene_add_bodies_force_creator(scene, (force_creator_t)drag_force_creator,
                                   drag_aux, bodies, (free_func_t)force_aux_free);
}

typedef struct collision_aux
{
    scene_t *scene;
    list_t *bodies;
    collision_handler_t handler;
    bool has_collided;
    void *aux;
    free_func_t aux_freer;

} collision_aux_t;

void collision_aux_free(collision_aux_t *c_aux)
{
    list_free(c_aux->bodies);
    if (c_aux->aux_freer != NULL)
    {
        c_aux->aux_freer(c_aux->aux);
    }
    free(c_aux);
}

void collision_creator(collision_aux_t *c_aux)
{
    assert(list_size(c_aux->bodies) == 2);
    body_t *body1 = list_get(c_aux->bodies, 0);
    body_t *body2 = list_get(c_aux->bodies, 1);

    // check distance between bodies is above the threshold (shortest
    // distance from centroid to vertex) to avoid extra math
    if (vec_distance(body_get_centroid(body1), body_get_centroid(body2)) > body_get_size(body1) + body_get_size(body2))
    {
        return;
    }

    collision_info_t collision = find_collision(body_get_shape(body1), body_get_shape(body2));
    bool currently_colliding = collision.collided;
    vector_t collision_axis = collision.axis;

    if (currently_colliding)
    {
        if (!c_aux->has_collided)
        {
            c_aux->has_collided = true;
            c_aux->handler(body1, body2, collision_axis, c_aux->aux);
        }
    }
    else
    {
        // not colliding
        c_aux->has_collided = false;
    }
}

void create_collision(scene_t *scene, body_t *body1, body_t *body2, collision_handler_t handler, void *aux, free_func_t freer)
{
    collision_aux_t *collision_aux = malloc(sizeof(collision_aux_t));
    assert(collision_aux != NULL);

    list_t *bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);

    *collision_aux = (collision_aux_t){.scene = scene, .bodies = bodies, .handler = handler, .has_collided = false, .aux = aux, .aux_freer = freer};
    scene_add_bodies_force_creator(scene, (force_creator_t)collision_creator, collision_aux, bodies, freer);
}

void one_sided_destroy(body_t *body1, body_t *body2, vector_t axis, void *aux)
{
    body_remove(body1);
}

void create_one_sided_destructive_collision(scene_t *scene, body_t *body1, body_t *body2)
{
    create_collision(scene, body1, body2, (collision_handler_t) one_sided_destroy, NULL, NULL);
}

void destroy(body_t *body1, body_t *body2, vector_t axis, void *aux)
{
    body_remove(body1);
    body_remove(body2);
}

void create_destructive_collision(scene_t *scene, body_t *body1, body_t *body2)
{
    create_collision(scene, body1, body2, (collision_handler_t)destroy, NULL, NULL);
}

typedef struct physics_aux
{
    double elasticity;

} physics_aux_t;

void physics_collide(body_t *body1, body_t *body2, vector_t axis, physics_aux_t *aux)
{
    double m1 = body_get_mass(body1);
    double m2 = body_get_mass(body2);
    double u1 = vec_dot(body_get_velocity(body1), axis);
    double u2 = vec_dot(body_get_velocity(body2), axis);
    double elasticity = aux->elasticity;

    double reduced_mass;
    if (m1 >= HUGE_VAL)
    {
        reduced_mass = m2;
    }
    else if (m2 >= HUGE_VAL)
    {
        reduced_mass = m1;
    }
    else
    {
        reduced_mass = (m1 * m2) / (m1 + m2);
    }
    double impulse_mag = reduced_mass * (1.0 + elasticity) * (u2 - u1);
    vector_t impulse = vec_multiply(impulse_mag, axis);
    body_add_impulse(body1, impulse);
    body_add_impulse(body2, vec_negate(impulse));
}

void create_physics_collision(scene_t *scene, double elasticity, body_t *body1, body_t *body2)
{
    physics_aux_t *p_aux = malloc(sizeof(physics_aux_t));
    assert(p_aux != NULL);
    p_aux->elasticity = elasticity;
    create_collision(scene, body1, body2, (collision_handler_t)physics_collide, p_aux, free);
}