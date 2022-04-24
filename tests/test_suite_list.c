#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "test_util.h"
#include "list.h"
#include "polygon.h"
#include "star.h"
#include "sdl_wrapper.h"

#define LARGE_SIZE 1000

void test_list_size0()
{
    list_t *l = list_init(0, free);
    assert(list_size(l) == 0);
    list_free(l);
}

void test_list_size1()
{
    list_t *l = list_init(1, (free_func_t)star_free);
    assert(list_size(l) == 0);
    // Add
    star_t *s = malloc(sizeof(*s));
    *s = *star_init_with_n(2);
    list_add(l, s);
    assert(list_size(l) == 1);
    // Remove
    assert(list_remove(l, list_size(l) - 1) == s);
    free(s);
    assert(list_size(l) == 0);
    // Add again
    s = malloc(sizeof(*s));
    *s = *star_init_with_n(3);
    list_add(l, s);
    assert(list_size(l) == 1);
    assert(star_equal(((star_t *)list_get(l, 0)), star_init_with_n(3)));
    // Modify
    *s = *star_init_with_n(4);
    assert(list_size(l) == 1);
    assert(star_equal(((star_t *)list_get(l, 0)), star_init_with_n(4)));
    list_free(l);
}

void test_list_small()
{
    list_t *l = list_init(5, (free_func_t)star_free);
    assert(list_size(l) == 0);
    // Fill partially
    star_t *s = malloc(sizeof(*s));
    *s = *star_init_with_n(2);
    list_add(l, s);
    s = malloc(sizeof(*s));
    *s = *star_init_with_n(3);
    list_add(l, s);
    s = malloc(sizeof(*s));
    *s = *star_init_with_n(4);
    list_add(l, s);
    assert(list_size(l) == 3);
    assert(star_equal(list_get(l, 0), star_init_with_n(2)));
    assert(star_equal(list_get(l, 1), star_init_with_n(3)));
    assert(star_equal(list_get(l, 2), star_init_with_n(4)));
    // Fill to capacity
    s = malloc(sizeof(*s));
    *s = *star_init_with_n(5);
    list_add(l, s);
    s = malloc(sizeof(*s));
    *s = *star_init_with_n(6);
    list_add(l, s);
    assert(list_size(l) == 5);
    assert(star_equal(list_get(l, 3), star_init_with_n(5)));
    assert(star_equal(list_get(l, 4), star_init_with_n(6)));
    // Remove some
    s = list_remove(l, list_size(l) - 1);
    assert(star_equal(s, star_init_with_n(6)));
    free(s);
    s = list_remove(l, list_size(l) - 1);
    assert(star_equal(s, star_init_with_n(5)));
    free(s);
    assert(list_size(l) == 3);
    // Add, replacing previous elements
    s = malloc(sizeof(*s));
    *s = *star_init_with_n(7);
    list_add(l, s);
    s = malloc(sizeof(*s));
    *s = *star_init_with_n(8);
    list_add(l, s);
    assert(list_size(l) == 5);
    assert(star_equal(list_get(l, 0), star_init_with_n(2)));
    assert(star_equal(list_get(l, 1), star_init_with_n(3)));
    assert(star_equal(list_get(l, 2), star_init_with_n(4)));
    assert(star_equal(list_get(l, 3), star_init_with_n(7)));
    assert(star_equal(list_get(l, 4), star_init_with_n(8)));
    // Overwrite added elements
    (*(star_t *)list_get(l, 3)) = *star_init_with_n(9);
    (*(star_t *)list_get(l, 4)) = *star_init_with_n(10);
    assert(star_equal(list_get(l, 0), star_init_with_n(2)));
    assert(star_equal(list_get(l, 1), star_init_with_n(3)));
    assert(star_equal(list_get(l, 2), star_init_with_n(4)));
    assert(star_equal(list_get(l, 3), star_init_with_n(9)));
    assert(star_equal(list_get(l, 4), star_init_with_n(10)));
    list_free(l);
}

void test_list_large_get_set()
{
    list_t *l = list_init(LARGE_SIZE, (free_func_t)star_free);
    // Add to capacity
    for (size_t i = 0; i < LARGE_SIZE; i++)
    {
        star_t *s = star_init_with_n(i + 2);
        list_add(l, s);
    }
    // Check
    for (size_t i = 0; i < LARGE_SIZE; i++)
    {
        assert(star_equal(list_get(l, i), star_init_with_n(i + 2)));
    }
    // Set every 100th value
    for (size_t i = 0; i < LARGE_SIZE; i += 100)
    {
        star_t *s = list_get(l, i);
        *s = *star_init_with_n((i * 10) + 2);
    }
    // Check all values again
    for (size_t i = 0; i < LARGE_SIZE; i++)
    {
        assert(star_equal(
            list_get(l, i),
            i % 100 == 0 ? star_init_with_n((i * 10) + 2) : star_init_with_n(i + 2)));
    }
    list_free(l);
}

void test_list_large_add_remove()
{
    list_t *l = list_init(LARGE_SIZE, (free_func_t)star_free);
    // Add to capacity
    for (size_t i = 0; i < LARGE_SIZE; i++)
    {
        star_t *s = star_init_with_n(i + 2);
        list_add(l, s);
    }
    // Remove all
    for (size_t i = 1; i <= LARGE_SIZE; i++)
    {
        size_t value = LARGE_SIZE - i;
        star_t *s = list_remove(l, list_size(l) - 1);
        assert(star_equal(s, star_init_with_n(value + 2)));
        free(s);
    }
    // Add to capacity again
    for (size_t i = 0; i < LARGE_SIZE; i++)
    {
        star_t *s = star_init_with_n(i + 3);
        list_add(l, s);
    }
    // Check all
    for (size_t i = 0; i < LARGE_SIZE; i++)
    {
        assert(star_equal(list_get(l, i), star_init_with_n(i + 3)));
    }

    list_t *l2 = list_init(LARGE_SIZE, (free_func_t)star_free);
    // Add to capacity
    for (size_t i = 0; i < LARGE_SIZE; i++)
    {
        vector_t *v = malloc(sizeof(*v));
        v->x = v->y = (double)i;
        list_add(l2, v);
    }
    // Remove all
    for (size_t i = 1; i <= LARGE_SIZE; i++)
    {
        double value = (double)(LARGE_SIZE - i);
        vector_t *v = list_remove(l2, list_size(l2) - 1);
        assert(vec_equal(*v, (vector_t){value, value}));
        free(v);
    }
    // Add to capacity again
    for (size_t i = 0; i < LARGE_SIZE; i++)
    {
        vector_t *v = malloc(sizeof(*v));
        v->x = v->y = (double)(i + 1);
        list_add(l2, v);
    }
    // Check all
    for (size_t i = 0; i < LARGE_SIZE; i++)
    {
        assert(vec_equal(*(vector_t *)list_get(l2, list_size(l2) - 1 - i), (vector_t){(double)(LARGE_SIZE - i), (double)(LARGE_SIZE - i)}));
    }
}

void test_list_resize()
{
    list_t *l = list_init(1, free);
    for (size_t i = 0; i < 2; i++)
    {
        vector_t *v = malloc(sizeof(*v));
        v->x = v->y = (double)i;
        list_add(l, v);
    }
    assert(list_capacity(l) == 2);
    free(l);
}

typedef struct
{
    list_t *list;
    size_t index;
} list_access_t;

void get_out_of_bounds(void *aux)
{
    list_access_t *access = (list_access_t *)aux;
    list_get(access->list, access->index);
}

void test_out_of_bounds_access()
{
    const size_t max_size = 5;
    list_access_t *access = malloc(sizeof(*access));
    access->list = list_init(max_size, (free_func_t)star_free);
    // This test takes several seconds to run
    fputs("test_out_of_bounds_access running...\n", stderr);

    // Try list with 0 elements, 1 element, ..., up to max_size elements
    for (size_t size = 0; size <= max_size; size++)
    {
        // Make sure negative indices report as out of bounds
        for (access->index = -3; (int)access->index < 0; access->index++)
        {
            assert(test_assert_fail(get_out_of_bounds, access));
        }

        // Make sure indices 0 through size - 1 are valid
        for (access->index = 0; access->index < size; access->index++)
        {
            // Store and retrieve an arbitrary vector
            star_t *new_star = star_init_with_n(access->index + 2);
            (*(star_t *)list_get(access->list, access->index)) = *new_star;
            assert(star_equal(list_get(access->list, access->index), new_star));
        }

        // Assert indices greater than or equal to size are invalid
        for (access->index = size; access->index < size + 3; access->index++)
        {
            assert(test_assert_fail(get_out_of_bounds, access));
        }

        // Increase the size of the list by 1
        if (size < max_size)
        {
            list_add(access->list, malloc(sizeof(star_t)));
        }
    }
    list_free(access->list);
    free(access);
}

void add_past_end(void *l)
{
    list_add((list_t *)l, malloc(sizeof(star_t)));
}
void test_full_add()
{
    const size_t size = 3;
    list_t *l = list_init(size, (free_func_t)star_free);

    // Fill list
    for (size_t i = 0; i < size; i++)
    {
        list_add(l, malloc(sizeof(star_t)));
    }

    // Try adding to the full list
    assert(!test_assert_fail(add_past_end, l));
}

void remove_from_empty(void *l)
{
    list_remove((list_t *)l, list_size(l) - 1);
}
void test_empty_remove()
{
    // TEST VEC_LIST
    const size_t size = 100;
    list_t *l = list_init(size, (free_func_t)star_free);

    // Fill list with copies of v, then remove them all
    vector_t v = {.x = 1, .y = -2};
    for (size_t i = 0; i < size; i++)
    {
        vector_t *list_v = malloc(sizeof(*list_v));
        *list_v = v;
        list_add(l, list_v);
    }
    for (size_t i = 0; i < size; i++)
    {
        vector_t *list_v = (vector_t *)list_remove(l, list_size(l) - 1);
        assert(vec_equal(*list_v, v));
        free(list_v);
    }

    // Try removing from the empty list -- should fail an assertion
    assert(test_assert_fail(remove_from_empty, l));

    list_free(l);
}

void add_null(void *l)
{
    list_add(l, NULL);
}
void test_null_values()
{
    list_t *l = list_init(1, (free_func_t)star_free);
    assert(test_assert_fail(add_null, l));
    list_free(l);
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

    DO_TEST(test_list_size0)
    DO_TEST(test_list_size1)
    DO_TEST(test_list_small)
    DO_TEST(test_list_large_get_set)
    DO_TEST(test_list_large_add_remove)
    DO_TEST(test_list_resize)
    DO_TEST(test_out_of_bounds_access)
    DO_TEST(test_full_add)
    DO_TEST(test_empty_remove)
    DO_TEST(test_null_values)

    puts("list_test PASS");
}