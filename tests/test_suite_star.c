#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "vector.h"
#include "polygon.h"
#include "test_util.h"
#include "star.h"
#include "sdl_wrapper.h"

// initial star conditions
const double TIME_DT = 1.0;
const vector_t POSITION = {.x = WINDOW_WIDTH / 2, .y = WINDOW_HEIGHT / 2};
const vector_t VELOCITY = {.x = 10.0, .y = 10.0};
const double RADIUS = 100.0;
const size_t POINTS = 5;
const double ROT_ANGLE = M_PI / 1052;
const rgb_color_t START_COLOR = {.r = 0.0, .g = 255.0, .b = 0.0};

void test_star_move_bounce()
{
    star_t *star = star_init(POSITION, VELOCITY, RADIUS, POINTS, ROT_ANGLE, START_COLOR);
    for (size_t i = 0; i < 1000; i++)
    {
        star_move_bounce(star, TIME_DT);
    }
    assert(vec_isclose(star->center, (vector_t){.x = 580, .y = 330}));
}

void test_star_move_gravity()
{
    star_t *star = star_init(POSITION, VELOCITY, RADIUS, POINTS, ROT_ANGLE, START_COLOR);
    for (size_t i = 0; i < 1000; i++)
    {
        star_move_gravity(star, TIME_DT);
    }
    assert(vec_magnitude(vec_subtract(star->center, (vector_t){.x = 10500.000000, .y = -810927.940102})) < 0.001);
}

void test_star_off_screen()
{
    star_t *star = star_init(POSITION, VELOCITY, RADIUS, POINTS, ROT_ANGLE, START_COLOR);
    // tests inside
    assert(!star_off_screen(star));

    // tests lower wall
    polygon_translate(star->vertices, (vector_t){.x = -WINDOW_WIDTH, .y = 0});
    assert(star_off_screen(star));
    polygon_translate(star->vertices, (vector_t){.x = WINDOW_WIDTH, .y = 0});

    // tests upper wall
    polygon_translate(star->vertices, (vector_t){.x = WINDOW_WIDTH, .y = 0});
    assert(star_off_screen(star));
    polygon_translate(star->vertices, (vector_t){.x = -WINDOW_WIDTH, .y = 0});

    // tests left wall
    polygon_translate(star->vertices, (vector_t){.x = 0, .y = -WINDOW_HEIGHT});
    assert(star_off_screen(star));
    polygon_translate(star->vertices, (vector_t){.x = 0, .y = WINDOW_HEIGHT});

    // tests right wall
    polygon_translate(star->vertices, (vector_t){.x = 0, .y = WINDOW_HEIGHT});
    assert(star_off_screen(star));
    polygon_translate(star->vertices, (vector_t){.x = 0, .y = -WINDOW_HEIGHT});
}

int main(int argc, char *argv[])
{
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests)
    {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_star_move_bounce)
    DO_TEST(test_star_move_gravity)
    DO_TEST(test_star_off_screen)

    puts("star_test PASS");
}
