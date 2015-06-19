#ifndef COMMON_H
#define COMMON_H

/* includes used in more than one test */
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

/* The filename of the device node, populated by populate_device() */
extern char *device_node;

/* verbose printing, activated by -v command-line arg */
extern bool verbose;
#define printv(fmt, ...) { if (verbose) printf(fmt, ##__VA_ARGS__); fflush(stdout); }

#define printe(fmt) { \
    int _e = errno; \
    printf("%s: error was %d (%s) - ", fmt, _e, strerror(_e)); \
    fflush(stdout); \
}

enum error_codes {
    ERR_NONE=0,
    ERR_ARGS,
    ERR_VERIFYING_NODE,
    ERR_EXEC,

    ERR_FAILURE=42 /* make sure this is always last */
};

void populate_device(int argc, char **argv);

int open_rdrw(void);
int open_rdonly(void);
int open_wronly(void);

#endif
