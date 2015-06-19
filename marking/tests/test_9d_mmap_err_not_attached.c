#include "common.h"
#include "marks.h"

#include <sys/ioctl.h>
#include "ioctl.h"

#include <sys/mman.h>

#define TOTAL_MARKS 1

int main(int argc, char **argv)
{
    populate_device(argc, argv);
    reset_marks(TOTAL_MARKS);

    int fd = open_rdrw();
    if (fd < 0) {
        goto end;
    }

    /* set up mmap */
    void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    int e = errno;
    if (ptr != MAP_FAILED) {
        printe("mmap did not fail as expected");
        goto end;
    }
    if (e == ENOTSUP || e == EOPNOTSUPP) {
        printv("mmap() failed with correct error, awarding 1 mark\n");
        update_marks(1);
        goto end;
    }

end:
    if (fd >= 0) {
        close(fd);
    }
    if (ptr != MAP_FAILED) {
        munmap(ptr, 4096);
    }
    return end_marks();
}
