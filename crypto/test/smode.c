#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../ioctl.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s device-node\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        perror("open");
        return 2;
    }

    struct crypto_smode m;
    m.dir = CRYPTO_WRITE;
    m.mode = CRYPTO_DEC;
    m.key = 0x3301;

    if (ioctl(fd, CRYPTO_IOCSMODE, &m) != 0) {
        perror("ioctl");
        return 3;
    }

    close(fd);
    return 0;
}
