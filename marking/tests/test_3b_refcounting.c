#include "common.h"
#include "marks.h"

#include <sys/ioctl.h>
#include "ioctl.h"

#define TOTAL_MARKS 3

int main(int argc, char **argv)
{
    populate_device(argc, argv);
    reset_marks(TOTAL_MARKS);

    int buf;

    int fd1 = open_rdonly();
    if (fd1 < 0) {
        goto end;
    }

    int fd2 = open_wronly();
    if (fd2 < 0) {
        goto end;
    }

    int ret = ioctl(fd1, CRYPTO_IOCCREATE, 0);
    if (ret < 0) {
        printe("ioctl(fd1, CRYPTO_IOCCREATE)");
        goto end;
    }
    printv("Created buffer with id %d\n", ret);
    device_dirty = true;
    buf = ret;

    ret = ioctl(fd2, CRYPTO_IOCTATTACH, buf);
    if (ret < 0) {
        printe("ioctl(fd2, CRYPTO_IOCTATTACH)");
        goto end;
    }

    /* 1 mark for having two fd's attached */
    update_marks(1);

    /* refcount drops */
    close(fd1);

    /* this should delete the buffer */
    ret = ioctl(fd2, CRYPTO_IOCDETACH, 0);
    if (ret < 0) {
        printe("ioctl(fd2, CRYPTO_IOCTDETACH)");
        goto end;
    }

    /* try attaching - this should fail */
    ret = ioctl(fd2, CRYPTO_IOCTATTACH, buf);
    if (ret == 0) {
        printe("attaching succeeded - buffer wasn't destroyed");
        goto end;
    }

    /* give final 2 marks */
    update_marks(2);

end:
    if (fd1 >= 0) {
        close(fd1);
    }
    if (fd2 >= 0) {
        close(fd2);
    }
    return end_marks();
}
