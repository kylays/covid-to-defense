#include "test_util.h"
#include <assert.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <io.h>
#include <setjmpex.h>
#include <signal.h>
#include <windows.h>
bool SIGABRT_RAISED = false;
jmp_buf env;
#else
#include <sys/wait.h>
#include <unistd.h>
#endif

bool color_equal(rgb_color_t c1, rgb_color_t c2)
{
    return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
}

bool star_equal(star_t *s1, star_t *s2)
{
    if (s1->num_points != s2->num_points)
    {
        return false;
    }
    if (s1->outer_radius != s2->outer_radius)
    {
        return false;
    }
    if (s1->inner_radius != s2->inner_radius)
    {
        return false;
    }
    if (s1->rot_angle != s2->rot_angle)
    {
        return false;
    }
    if (!vec_equal(s1->center, s2->center))
    {
        return false;
    }
    if (!vec_equal(s1->velocity, s2->velocity))
    {
        return false;
    }
    if (!color_equal(s1->color, s2->color))
    {
        return false;
    }
    if (list_size(s1->vertices) != list_size(s2->vertices))
    {
        return false;
    }
    for (size_t i = 0; i < list_size(s1->vertices); i++)
    {
        if (!vec_equal((*(vector_t *)list_get(s1->vertices, i)), (*(vector_t *)list_get(s2->vertices, i))))
        {
            return false;
        }
    }
    return true;
}

bool within(double epsilon, double d1, double d2){
    return fabs(d1 - d2) < epsilon;
}

bool isclose(double d1, double d2) {
    return within(1e-7, d1, d2);
}

bool vec_within(double epsilon, vector_t v1, vector_t v2) {
    return within(epsilon, v1.x, v2.x) && within(epsilon, v1.y, v2.y);
}

bool vec_equal(vector_t v1, vector_t v2) {
    return v1.x == v2.x && v1.y == v2.y;
}

bool vec_isclose(vector_t v1, vector_t v2) {
    return isclose(v1.x, v2.x) && isclose(v1.y, v2.y);
}

void read_testname(char* filename, char* testname, size_t testname_size) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        printf("Couldn't open file %s\n", filename);
        exit(1);
    }
    // Generate format string
    char fmt[12];
    snprintf(fmt, sizeof(fmt), "%%%lus", (unsigned long)testname_size - 1);
    fscanf(f, fmt, testname);
    fclose(f);
}

#ifdef _WIN32
void signal_handler(int signum) {
    if (signum == SIGABRT) {
        SIGABRT_RAISED = true;
        // Unregister self, and tell kernel to ignore
        signal(signum, SIG_DFL);

        // Jump out to avoid abort call after signal_handler returns
        // Safe-ish, since our code is (probably) dead simple?
        // Some comments
        // https://gist.github.com/kekyo/cc9bace942b8c2aa2484431e047d267d
        // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/signal?view=vs-2019#remarks
        longjmp(env, 1);
    }
}
#endif

bool test_assert_fail(void (*run)(void* aux), void* aux) {
    // Windows can't use POSIX apis
#ifndef _WIN32
    if (fork()) { // parent process
        int* status = malloc(sizeof(*status));
        assert(status != NULL);
        wait(status);
        // Check whether child process aborted
        bool aborted = WIFSIGNALED(*status) && WTERMSIG(*status) == SIGABRT;
        free(status);
        return aborted;
    }
    else {                             // child process
        freopen("/dev/null", "w", stderr); // suppress assertion message
        run(aux);
        exit(0); // should not be reached
    }
#else
    // NOTE: this is super sketch. A "better" alternative may be to just not run
    // at all.

    // Register signal handler to trap SIGABRT from assert
    signal(SIGABRT, signal_handler);
    SIGABRT_RAISED = false;
    // Store another file descriptor for stderr so that we can get it back later
    int cstderr = _dup(_fileno(stderr));

    // Run expected failure, jumping back when SIGABRT is raised
    if (setjmp(env) == 0) {
        // Suppress assertion message by piping to NUL
        freopen("NUL", "w", stderr);
        run(aux);
    }

    // Since we are in the same process, undo the suppression by reassigning
    // stderr to the saved descriptor
    _dup2(cstderr, _fileno(stderr));

    // Return whether we failed or not.
    return SIGABRT_RAISED;
#endif
}
