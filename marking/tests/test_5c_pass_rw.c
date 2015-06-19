#include "common.h"
#include "marks.h"

#include <sys/ioctl.h>
#include "ioctl.h"

#define TOTAL_MARKS 3

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
    char *str = "this is a test for 5c! ~!@#$2343....\n";
    int len = strlen(str);

    /* set encrypt on write */
    struct crypto_smode sw;
    sw.dir = CRYPTO_WRITE;
    sw.mode = CRYPTO_PASSTHROUGH;
    ret = ioctl(fd, CRYPTO_IOCSMODE, &sw);
    if (ret != 0) {
        printe("failed to set write mode to passthrough");
        goto end;
    }

    /* write to the buffer */
    if (write(fd, str, len) != len) {
        printe("write did not return correct number of bytes");
        goto end;
    }

    struct crypto_smode sr;
    sr.dir = CRYPTO_READ;
    sr.mode = CRYPTO_PASSTHROUGH;
    ret = ioctl(fd, CRYPTO_IOCSMODE, &sr);
    if (ret != 0) {
        printe("failed to set read mode to passthrough");
        goto end;
    }

    /* try and read it back */
    int bytes;
    char buf[1024];
    bytes = read(fd, buf, sizeof(buf));
    if (bytes != len) {
        printe("read did not return correct number of bytes");
        goto end;
    }
    printv("Read correct number of bytes (%d) from device, awarding 1 mark\n", bytes);
    update_marks(1);

    printv("checking string\n");
    buf[bytes] = '\0';
    if (strcmp(buf, str) != 0) {
        printe("string was not the same");
        printv("  original: \"%s\"\n", str);
        printv("  assert  : \"%s\"\n", buf);
        goto end;
    }
    printv("Strings were identical, awarding 2 marks\n");
    update_marks(2);

end:
    if (fd >= 0) {
        close(fd);
    }
    return end_marks();
}
