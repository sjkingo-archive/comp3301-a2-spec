#include "common.h"
#include "marks.h"

#include <sys/ioctl.h>
#include "ioctl.h"

#define TOTAL_MARKS 10

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
    char *str = "this is a test for 5a and 5b";
    int len = strlen(str);

    /* set encrypt on write */
    struct crypto_smode sw;
    sw.dir = CRYPTO_WRITE;
    sw.mode = CRYPTO_ENC;
    sw.key = 0x12;
    ret = ioctl(fd, CRYPTO_IOCSMODE, &sw);
    if (ret != 0) {
        printe("failed to set write mode to encrypt");
        goto end;
    }

    /* if setting write mode to encrypt worked, then award 2 marks */
    printv("Set write mode to encrypt, awarding 2 marks\n");
    update_marks(2);
    
    /* write to the buffer */
    if (write(fd, str, len) != len) {
        printe("write did not return correct number of bytes");
        goto end;
    }
    printv("Wrote %d bytes to device, awarding 3 marks\n", len);
    update_marks(3);

    /* @@ END OF 5a @@ */
    printv("@@ 5b..\n");

    /* set read mode to decrypt */
    struct crypto_smode sr;
    sr.dir = CRYPTO_READ;
    sr.mode = CRYPTO_DEC;
    sr.key = 0x12;
    ret = ioctl(fd, CRYPTO_IOCSMODE, &sr);
    if (ret != 0) {
        printe("failed to set read mode to decrypt");
        goto end;
    }

    /* if setting read mode to decrypt worked, then award 2 marks */
    printv("Set read mode to decrypt, awarding 2 marks\n");
    update_marks(2);

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

    printf("5 marks of this goes to 5b - ");

end:
    if (fd >= 0) {
        close(fd);
    }
    return end_marks();
}
