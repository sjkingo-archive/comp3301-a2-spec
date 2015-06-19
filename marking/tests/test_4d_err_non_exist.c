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

    int ret = ioctl(fd, CRYPTO_IOCTATTACH, 4242);
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
