#include "common.h"
#include "marks.h"

#include <sys/ioctl.h>
#include "ioctl.h"

#include <sys/mman.h>

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
    char *str = "1122!! this is a test for 9a! ~!@e3....\n";
    int len = strlen(str);

    /* set write mode to passthrough and write to the buffer */
    struct crypto_smode sw;
    sw.dir = CRYPTO_WRITE;
    sw.mode = CRYPTO_PASSTHROUGH;
    ret = ioctl(fd, CRYPTO_IOCSMODE, &sw);
    if (ret != 0) {
        printe("failed to set write mode to passthrough");
        goto end;
    }
    if (write(fd, str, len) != len) {
        printe("write did not return correct number of bytes");
        goto end;
    }

    /* set up mmap */
    void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        printe("mmap failed");
        goto end;
    }
    printv("mmap() succeeded with ptr %p, awarding 1 mark\n", ptr);
    update_marks(1);

    printv("checking string\n");
    /* we use strncmp since ptr won't be NUL-terminated */
    if (strncmp((char *)ptr, str, len) != 0) {
        printe("string was not the same");
        printv("  original: \"%s\"\n", str);
        printv("  assert  : \"%s\"\n", (char *)ptr);
        goto end;
    }
    printv("Strings were identical, awarding 1 mark\n");
    update_marks(1);

end:
    if (fd >= 0) {
        close(fd);
    }
    if (ptr != MAP_FAILED) {
        munmap(ptr, 4096);
    }
    return end_marks();
}
