#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "test.h"

static void usage_error(void)
{
    fprintf(stderr, "Usage: test TEST device\n\n");
    fprintf(stderr, "where TEST is one of the following, specifying which "
            "test to run:\n");
    fprintf(stderr, "  --all\t\trun all tests (in the following order)\n");

    int i;
    for (i = 0; i < callbacks_len(); i++) {
        struct test_case t = test_callbacks[i];
        fprintf(stderr, "  --%s\t\t%s (id %d)\n", t.name, t.desc, t.id);
    }

    exit(1);
}

static char *parse_args(int argc, char **argv)
{
    if (argc != 3) {
        usage_error();
    }

    current_test = TEST_INVALID;

    if (strcmp(argv[1], "--all") == 0) {
        current_test = TEST_ALL;
    } else {
        int i;
        for (i = 0; i < callbacks_len(); i++) {
            struct test_case t = test_callbacks[i];
            if (strcmp(t.name, argv[1] + 2) == 0) {
                current_test = t.id;
                break;
            }
        }
    }

    return argv[2]; /* device */
}

static int open_device(char *filename)
{
    int fd = open(filename, O_RDWR);
    if (fd < 0) {
        perror("open");
        exit(2);
    }
    return fd;
}

static void close_device(int fd)
{
    close(fd);
}

int main(int argc, char **argv)
{
    char *dev = parse_args(argc, argv);
    if (current_test == TEST_INVALID) {
        fprintf(stderr, "Invalid test option %s\n", argv[1]);
        return 1;
    }

    int fd = open_device(dev);
    int failures = dispatch_test(fd);
    close_device(fd);

    if (failures != 0) {
        fprintf(stderr, "Failed %d test(s)\n", failures);
    } else {
        fprintf(stderr, "Passed all tests\n");
    }

    return 0;
}
