#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "test.h"

/* Test prototypes for use in test_callbacks */
static bool empty_test(int);
static bool rw_test(int);

/* All of the tests in a callback array */
struct test_case test_callbacks[] = {
    { TEST_EMPTY, "empty", "open and close only", empty_test },
    { TEST_RW, "rw", "read/write test", rw_test },
};

/* Length of the callbacks array */
#define CALLBACKS_LEN (sizeof(test_callbacks) / sizeof(struct test_case))
int callbacks_len(void)
{
    return CALLBACKS_LEN;
}

/* Current test type as set by parse_args() */
enum test_type current_test = TEST_INVALID;

static void run_test(struct test_case *t, int *failures, int fd)
{
    bool ret = t->callback(fd);
    if (!ret) {
        *failures += 1;
        fprintf(stderr, "Test \"%s\" (%s) FAILED\n", t->name, t->desc);
    } else {
        fprintf(stderr, "Test \"%s\" (%s) PASSED\n", t->name, t->desc);
    }
    fprintf(stderr, "--\n");
}

int dispatch_test(int fd)
{
    int failures = 0;

    if (current_test == TEST_ALL) {
        int i;
        for (i = 0; i < CALLBACKS_LEN; i++) {
            run_test(&test_callbacks[i], &failures, fd);
        }
    } else {
        assert(current_test <= CALLBACKS_LEN);
        run_test(&test_callbacks[current_test], &failures, fd);
    }

    return failures;
}

/* Actual testing functions, these should all be static */

static bool empty_test(int fd)
{
    return true;
}

static bool rw_test(int fd)
{
    char buf[10];
    int bytes;
    char *str = "hello";
    int len = strlen(str);

    /* write a string to the device */
    bytes = write(fd, str, len);
    if (!test_int(bytes, len)) {
        return false;
    }

    /* read the same string back and make sure it's the same */
    bytes = read(fd, buf, sizeof(buf)-1);
    if (!test_int(bytes, len)) {
        return false;
    }
    buf[bytes] = '\0';
    if (!test_str(str, buf)) {
        return false;
    }

    return true;
}
