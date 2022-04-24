#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "test_util.h"
#include "sdl_wrapper.h"
#include "forces.h"
#include "vector.h"
#include "collision.h"
#include "body.h"

// body_t* make_squre_with_side_length(double side_length)
// {
//     list_t *shape = list_init(5, free);
//     vector_t *v = malloc(sizeof(vector_t));
//     *v = (vector_t){0, 0};
//     list_add(shape, v);
//     vector_t *v1 = malloc(sizeof(vector_t));
//     *v1 = (vector_t){0, side_length};
//     list_add(shape, v1);
//     vector_t *v2 = malloc(sizeof(vector_t));
//     *v2 = (vector_t){side_length, side_length};
//     list_add(shape, v2);
//     vector_t *v3 = malloc(sizeof(vector_t));
//     *v3 = (vector_t){side_length, 0};
//     list_add(shape, v3);

//     body_t *body = body_init(shape, 1, 0, {.r = 0, .g = 0, .b = 0});
//     return body;
// }
// bool check_collision(body_t* body_1, body_t* body_2) {
//     return find_collision(body_get_shape(body_1), body_get_shape(body_2));
// }

// void sanity_test_collision()
// {
//     body_t* b1 = make_squre_with_side_length(5);
//     body_t* b2 = make_squre_with_side_length(5);
//     assert(check_collision(b1, b2));
//     body_free(b1);
//     body_free(b2);
// }
// void test_collisions()
// {
//     body_t* b1 = make_squre_with_side_length(5);
//     body_t* b2 = make_squre_with_side_length(5);
//     assert(check_collision(b1, b2) == false);

//     body_set_centroid(b2, (vector_t) {.x = 11.0, .y = 11.0});
//     assert(check_collision(b1, b2) == false);

//     body_set_centroid(b2, (vector_t) {.x = 9.0, .y = 9.0});
//     assert(check_collision(b1, b2) == true);

//     body_set_centroid(b2, (vector_t) {.x = -9.0, .y = 9.0});
//     assert(check_collision(b1, b2) == true);

//     body_set_centroid(b2, (vector_t) {.x = -11.0, .y = 0});
//     assert(check_collision(b1, b2) == false);

//     body_free(b1);
//     body_free(b2);
// }

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

    // DO_TEST(sanity_test_collision);
    // DO_TEST(test_collisions);

    puts("collision_tests nothing PASS");
}
