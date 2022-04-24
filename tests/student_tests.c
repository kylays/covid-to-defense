#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "test_util.h"
#include "sdl_wrapper.h"
#include "forces.h"
#include "vector.h"

// CONSTS
const rgb_color_t COLOR = {.r = 0.0, .g = 0.0, .b = 0.0};

list_t *make_shape()
{
    list_t *shape = list_init(5, free);
    vector_t *v = malloc(sizeof(vector_t));
    *v = (vector_t){-1, -1};
    list_add(shape, v);
    vector_t *v1 = malloc(sizeof(vector_t));
    *v1 = (vector_t){+1, -1};
    list_add(shape, v1);
    vector_t *v2 = malloc(sizeof(vector_t));
    *v2 = (vector_t){+1, +1};
    list_add(shape, v2);
    vector_t *v3 = malloc(sizeof(vector_t));
    *v3 = (vector_t){-1, +1};
    list_add(shape, v3);
    return shape;
}

// 1. NEWTONIAN GRAVITY
// tests simulating orbital path trajectory
const double G = 6.67 * 10E-11;
double centripetal_velocity(body_t *EARTH, body_t *satellite)
{
    double R = vec_distance(body_get_centroid(EARTH), body_get_centroid(satellite));
    return pow(G * body_get_mass(EARTH) / R, 0.5);
}

void test_centripetal_force()
{
    const double EARTH_MASS = 10000;
    const double M = 1;
    const double DT = 3;
    const int STEPS = 1000;

    scene_t *scene = scene_init();

    body_t *EARTH = body_init(make_shape(), EARTH_MASS, COLOR);
    scene_add_body(scene, EARTH);

    body_t *satellite = body_init(make_shape(), M, COLOR);
    body_set_centroid(satellite, (vector_t){5.0, 0.0});
    double satellite_vel = centripetal_velocity(EARTH, satellite);

    body_set_velocity(satellite, (vector_t){0.0, satellite_vel});
    scene_add_body(scene, satellite);

    create_newtonian_gravity(scene, G, EARTH, satellite);

    double standard_distance = vec_distance(body_get_centroid(satellite), body_get_centroid(EARTH));

    for (int i = 0; i < STEPS; i++)
    {
        assert(within(1e-2, vec_distance(body_get_centroid(satellite), body_get_centroid(EARTH)) - standard_distance, 0));
        scene_tick(scene, DT);
        body_set_centroid(EARTH, VEC_ZERO);
        body_set_velocity(EARTH, VEC_ZERO);
    }
    scene_free(scene);
}

// 2. DRAG TEST
// tests that drag is implemented correctly by measuring the trajectory of a moving body
void test_drag_moving_body()
{
    const double M1 = 40;
    const double DT = 1e-3;
    const int STEPS = 100000;
    double GAMMA = 10;

    scene_t *scene = scene_init();
    body_t *mass = body_init(make_shape(), M1, COLOR);
    vector_t initial_vel = {15.0, 15.0};
    body_set_velocity(mass, initial_vel);
    scene_add_body(scene, mass);

    create_drag(scene, GAMMA, mass);

    double old_vel = vec_magnitude(initial_vel);

    for (int i = 0; i < STEPS; i++)
    {
        assert(old_vel - vec_magnitude(body_get_velocity(mass)) >= 0);
        old_vel = vec_magnitude(body_get_velocity(mass));
        scene_tick(scene, DT);
    }
    assert(vec_isclose(body_get_velocity(mass), VEC_ZERO));

    scene_free(scene);
}

// 3. SPRING TEST
// tests create_spring by using the law of conservation of energy
double kinetic_energy(body_t *body)
{
    vector_t v = body_get_velocity(body);
    return body_get_mass(body) * vec_dot(v, v) / 2;
}

double spring_potential(double k, body_t *body1, body_t *body2)
{
    double distance = vec_distance(body_get_centroid(body1), body_get_centroid(body2));
    return 0.5 * k * pow(distance, 2);
}

void test_spring_energy_conservation()
{
    const double M1 = 40, M2 = 7.3;
    const double K = 22;
    const double DT = 1e-6;
    const int STEPS = 1000000;
    scene_t *scene = scene_init();
    body_t *mass1 = body_init(make_shape(), M1, (rgb_color_t){0, 0, 0});
    scene_add_body(scene, mass1);
    body_t *mass2 = body_init(make_shape(), M2, (rgb_color_t){0, 0, 0});
    body_set_centroid(mass2, (vector_t){10, 20});
    scene_add_body(scene, mass2);
    create_spring(scene, G, mass1, mass2);

    double initial_energy = spring_potential(K, mass1, mass2) + kinetic_energy(mass1) + kinetic_energy(mass2);

    for (int i = 0; i < STEPS; i++)
    {
        assert(body_get_centroid(mass1).x < body_get_centroid(mass2).x);
        double curr_energy = spring_potential(K, mass1, mass2) + kinetic_energy(mass1) +
                             kinetic_energy(mass2);
        assert(within(1e-4, curr_energy / initial_energy, 1));
        scene_tick(scene, DT);
    }
    scene_free(scene);
}

int main(int argc, char *argv[])
{
    // Run all tests? True if there are no command-line arguments
    bool all_tests = argc == 1;

    // Read test name from file
    char testname[100];
    if (!all_tests)
    {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_centripetal_force)
    DO_TEST(test_drag_moving_body)
    DO_TEST(test_spring_energy_conservation)

    puts("student_tests PASS");
}
