#include "common.h"

#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#define USAGE "Usage: %s [-v] device-node\n"

bool verbose = false;
char *device_node = NULL;

void populate_device(int argc, char **argv)
{
    /* extract command-line arguments */
    if (argc == 2) {
        device_node = argv[1];
    } else if (argc == 3) {
        if (strcmp(argv[1], "-v") == 0) {
            verbose = true;
        } else {
            fprintf(stderr, USAGE, argv[0]);
            exit(ERR_ARGS);
        }
        device_node = argv[2];
    } else {
        fprintf(stderr, USAGE, argv[0]);
        exit(ERR_ARGS);
    }

    assert(device_node != NULL); /* to ensure I haven't broken args handling */
    printv("Using device node `%s`\n", device_node);
    
    /* test if the filename exists and is a character device */
    struct stat sb;
    if (stat(device_node, &sb) < 0) {
        fprintf(stderr, "Could not stat() `%s`: %s\n", device_node,
                strerror(errno));
        exit(ERR_VERIFYING_NODE);
    }
    if (!S_ISCHR(sb.st_mode)) {
        fprintf(stderr, "`%s` exists but is not a character device\n",
                device_node);
        exit(ERR_VERIFYING_NODE);
    }
}

int open_rdrw(void)
{
    printv("Trying to open(O_RDWR)\n");
    int fd = open(device_node, O_RDWR);
    if (fd < 0) {
        perror("open(O_RDWR)");
    }
    return fd;
}

int open_rdonly(void)
{
    printv("Trying to open(O_RDONLY)\n");
    int fd = open(device_node, O_RDONLY);
    if (fd < 0) {
        perror("open(O_RDONLY)");
    }
    return fd;
}

int open_wronly(void)
{
    printv("Trying to open(O_WRONLY)\n");
    int fd = open(device_node, O_WRONLY);
    if (fd < 0) {
        perror("open(O_WRONLY)");
    }
    return fd;
}
