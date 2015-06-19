#include "common.h"
#include "marks.h"

#include <sys/ioctl.h>
#include "ioctl.h"

#define TOTAL_MARKS 2

int main(int argc, char **argv)
{
    populate_device(argc, argv);
    reset_marks(TOTAL_MARKS);

    int fd = open_rdrw();
    if (fd < 0) {
        goto end;
    }

    int ret;
    for (int i = 0; i < 100; i++) {
        ret = ioctl(fd, CRYPTO_IOCCREATE, 0);
        if (ret < 0) {
            printe("ioctl(CRYPTO_IOCCREATE)");
            goto end;
        }

        device_dirty = true;

        if (i == 25) {
            /* half marks for this many */
            printv("created 25 buffers, awarding 1 mark\n");
            update_marks(1);
        }

        ioctl(fd, CRYPTO_IOCDETACH, 0);
    }

    /* if we got here, all 100 were created */
    printv("created 100 buffers, awarding 1 mark\n");
    printv("(last buffer id was %d)\n", ret);
    update_marks(1);

end:
    if (fd >= 0) {
        close(fd);
    }
    return end_marks();
}
