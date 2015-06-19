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

    /* set up mmap */
    void *ptr = mmap(NULL, 4096*4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    int e = errno;
    if (ptr != MAP_FAILED) {
        printe("mmap did not fail as expected");
        goto end;
    }
    if (e == EIO) {
        printv("mmap() failed with correct error, awarding 2 marks\n");
        update_marks(2);
        goto end;
    }

end:
    if (fd >= 0) {
        close(fd);
    }
    if (ptr != MAP_FAILED) {
        munmap(ptr, 4096*4);
    }
    return end_marks();
}
