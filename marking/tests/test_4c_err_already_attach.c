#include "common.h"
#include "marks.h"

#include <sys/ioctl.h>
#include "ioctl.h"

#define TOTAL_MARKS 1

int main(int argc, char **argv)
{
    populate_device(argc, argv);
    reset_marks(TOTAL_MARKS);

    int fd = open_rdrw();
    if (fd < 0) {
        goto end;
    }

    int ret = ioctl(fd, CRYPTO_IOCCREATE, 0);
    if (ret < 0) {
        printe("ioctl(CRYPTO_IOCCREATE)");
        goto end;
    }
    printv("Created buffer with id %d\n", ret);
    device_dirty = true;

    ret = ioctl(fd, CRYPTO_IOCTATTACH, ret);
    if (ret == 0) {
        printe("attached when shouldn't be");
        goto end;
    }

    update_marks(1);

end:
    if (fd >= 0) {
        close(fd);
    }
    return end_marks();
}
