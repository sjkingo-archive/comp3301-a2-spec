#include "common.h"
#include "marks.h"

#include <sys/ioctl.h>
#include <sys/wait.h>
#include "ioctl.h"

#define TOTAL_MARKS 2

int create_buffer(int i) {
    /* DO NOT CALL ANY MARKS FUNCTIONS HERE */

    int ret = -1;

    int fd = open_rdrw();
    if (fd < 0) {
        ret = 4;
        goto end;
    }

    ret = ioctl(fd, CRYPTO_IOCCREATE, 0);
    if (ret < 0) {
        fprintf(stderr, "ioctl(CRYPTO_IOCCREATE) #%d", i);
        printe("");
        ret = 5;
        goto end;
    }
    printv("Created buffer with id %d\n", ret);
    ret = 0;

end:
    if (fd >= 0) {
        close(fd);
    }
    return ret;
}

int main(int argc, char **argv)
{
    populate_device(argc, argv);
    reset_marks(TOTAL_MARKS);

    int ret;

    int pid = fork();
    if (pid == 0) {
        printv("Child sleeping for 3 seconds\n");
        sleep(3);
        printv("Child creating buffer\n");
        ret = create_buffer(1);
        printv("Child finished, exiting with status %d\n", ret);
        _exit(ret);
    } else {
        printv("Parent creating buffer\n");
        ret = create_buffer(0);
        if (ret == 0) {
            update_marks(1);
        }
        if (ret != 4) {
            device_dirty = true;
        }

        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            int child_ret = WEXITSTATUS(status);
            printv("Child returned status %d\n", child_ret);
            if (child_ret == 0) {
                update_marks(1);
            }
            if (ret != 4) {
                device_dirty = true;
            }
        }
    }

    /* only the parent gets here */
    return end_marks();
}
