#ifndef TEST_H
#define TEST_H

#define SHOW_SUCCESS 1

#include <stdbool.h>
#include <stdio.h> /* for printf() */
#include <string.h> /* for strcmp() in test_str() macro */

#define test_int(e, a) _test_int(__FILE__, __LINE__, e, a)
static inline bool _test_int(char *f, int l, int e, int a)
{
    if (e != a) {
        printf("FAILURE at %s:%d: actual %d != expected %d\n", f, l, a, e);
        return false;
#if SHOW_SUCCESS == 1
    } else {
        printf("SUCCESS at %s:%d: %d\n", f, l, e);
#endif
    }
    return true;
}

#define test_str(e, a) _test_str(__FILE__, __LINE__, e, a)
static inline bool _test_str(char *f, int l, char *e, char *a)
{
    if (strcmp(e, a) != 0) {
        printf("FAILURE at %s:%d: actual \"%s\" != expected \"%s\"\n", f, l, a, e);
        return false;
#if SHOW_SUCCESS == 1
    } else {
        printf("SUCCESS at %s:%d: \"%s\"\n", f, l, e);
#endif
    }
    return true;
}

enum test_type {
    TEST_EMPTY=0,
    TEST_RW,

    /* must be at the bottom! */
    TEST_ALL=998,
    TEST_INVALID=999
};
extern enum test_type current_test;

struct test_case {
    int id;
    char *name;
    char *desc;
    bool (*callback)(int);
};
extern struct test_case test_callbacks[];

int callbacks_len(void);
int dispatch_test(int);

#endif
