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

    int ret = ioctl(fd, CRYPTO_IOCCREATE, 0);
    if (ret < 0) {
        printe("ioctl(CRYPTO_IOCCREATE)");
        goto end;
    }
    printv("Created buffer with id %d\n", ret);
    device_dirty = true;

    /* our test string */
    char *str = "this is a test for 6a and 6b";
    int len = strlen(str);

    /* write to the buffer */
    if (write(fd, str, len) != len) {
        printe("write did not return correct number of bytes");
        goto end;
    }
    update_marks(1);

    /* try and read it back */
    int bytes;
    char buf[1024];
    bytes = read(fd, buf, sizeof(buf));
    if (bytes != len) {
        printe("read did not return correct number of bytes");
        goto end;
    }
    buf[bytes] = '\0';
    if (strcmp(buf, str) != 0) {
        printe("string was not the same");
        goto end;
    }
    update_marks(1);

    printf("1 mark of this goes to 6b - ");

end:
    if (fd >= 0) {
        close(fd);
    }
    return end_marks();
}
