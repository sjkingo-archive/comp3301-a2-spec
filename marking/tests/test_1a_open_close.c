#include "common.h"
#include "marks.h"

#define TOTAL_MARKS 2

int main(int argc, char **argv)
{
    populate_device(argc, argv);
    reset_marks(TOTAL_MARKS);

    /* 1 mark: open. Device must be able to be opened for reading, writing and
     * reading/writing to get this mark.
     */
    int fd = open_rdrw();
    if (fd < 0) {
        goto try_close;
    }

    fd = open_rdonly();
    if (fd < 0) {
        goto try_close;
    }

    fd = open_wronly();
    if (fd < 0) {
        goto try_close;
    }

    /* if we got here, then all of the open() calls worked */
    update_marks(1);

try_close:
    /* 1 mark: close. If open() did not work, then the student gets 0 for this
     * criteria too.
     */
    if (fd < 0) {
        fprintf(stderr, "Since all opens failed, close() cannot be tested)\n");
        assert(get_marks() == 0);
    } else {
        if (close(fd) == 0) {
            update_marks(1);
        } else {
            printe("close()");
        }
    }

    return end_marks();
}
