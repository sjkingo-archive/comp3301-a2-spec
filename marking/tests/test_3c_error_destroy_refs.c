#include "common.h"
#include "marks.h"

#include <sys/ioctl.h>
#include <sys/wait.h>
#include "ioctl.h"

#define TOTAL_MARKS 1

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
    int p[2];

    pipe(p);

    int pid = fork();
    if (pid == 0) {
        close(p[1]); /* close write */

        int fd = open_rdrw();
        if (fd < 0) {
            ret = 4;
            goto child_out;
        }

        printv("Child waiting for parent's buffer id\n");
        int id = -1;
        if (read(p[0], &id, sizeof(id)) != sizeof(id)) {
            printe("failed to read from parent");
            ret = errno;
            goto child_out;
        }

        int err;
        printv("Child attempting to destroy parent's buffer with id %d\n", id);
        ret = ioctl(fd, CRYPTO_IOCTDELETE, id);
        err = errno;
        if (ret != -1) {
            printf("ioctl didn't fail as it should have\n");
            ret = 5;
            goto child_out;
        }
        if (err == ENOTSUP || err == EOPNOTSUPP || err == EINVAL) {
            printv("Correct error returned\n");
            ret = 0;
        } else {
            printf("incorrect error returned, should not have been: %d (%s) - ", errno, strerror(errno));
            ret = 5;
            goto child_out;
        }

child_out:
        printv("Child finished, exiting with status %d\n", ret);
        close(p[0]);
        _exit(ret);
    } else {
        close(p[0]); /* close read */

        printv("Parent creating buffer\n");
        ret = create_buffer(0);
        if (ret != 4) {
            device_dirty = true;
        }

        printv("Parent sending buffer id to child\n");
        if (write(p[1], &ret, sizeof(ret)) != sizeof(ret)) {
            printe("failed to write to child\n");
            goto parent_out;
        }

        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            int child_ret = WEXITSTATUS(status);
            printv("Child returned status %d\n", child_ret);
            if (child_ret == 0) {
                update_marks(1);
            }
        }
    }

parent_out:
    /* only the parent gets here */
    close(p[1]);
    return end_marks();
}
