#include "common.h"
#include "marks.h"

#include <sys/ioctl.h>
#include "ioctl.h"

#define TOTAL_MARKS 1

int main(int argc, char **argv)
{
    populate_device(argc, argv);
    reset_marks(TOTAL_MARKS);

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
    int buf = ret;
    printv("Created buffer with id %d\n", ret);
    device_dirty = true;

    ret = ioctl(fd2, CRYPTO_IOCTATTACH, buf);
    if (ret != 0) {
        printe("ioctl(fd2, CRYPTO_IOCTATTACH)");
        goto end;
    }
    printv("Attached to buffer with id %d\n", buf);

    update_marks(1);

end:
    if (fd1 >= 0) {
        close(fd1);
    }
    if (fd2 >= 0) {
        close(fd2);
    }
    return end_marks();
}
