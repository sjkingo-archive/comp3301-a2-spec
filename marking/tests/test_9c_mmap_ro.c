#include "common.h"
#include "marks.h"

#include <sys/ioctl.h>
#include "ioctl.h"

#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>

#define TOTAL_MARKS 2

int main(int argc, char **argv)
{
    populate_device(argc, argv);
    reset_marks(TOTAL_MARKS);

    /* we fork here since the child *should* segfault */
    int pid = fork();
    if (pid == 0) {
        int fd = open_rdrw();
        if (fd < 0) {
            goto end;
        }

        int ret = ioctl(fd, CRYPTO_IOCCREATE, 0);
        if (ret < 0) {
            printe("ioctl(CRYPTO_IOCCREATE)");
            _exit(1);
        }
        printv("Created buffer with id %d\n", ret);

        /* set up mmap */
        void *ptr = mmap(NULL, 4096, PROT_READ, MAP_SHARED, fd, 0);
        if (ptr == MAP_FAILED) {
            printe("mmap failed");
            _exit(2);
        }

        /* this should segfault */
        ((char *)ptr)[0] = 'a';
        
        /* should never get here */
        _exit(0);
    } else {
        int status;
        wait(&status);
        device_dirty = true;
        if (WIFSIGNALED(status)) {
            if (WTERMSIG(status) == SIGSEGV) { /* segfault.. good! */
                printv("child segfaulted as expected\n");
                update_marks(2);
            } else {
                printe("child did not segfault, but was signalled");
            }
        } else {
            printe("child exited normally, expected a segfault");
            printv("killing child\n");
            kill(pid, SIGKILL);
        }
    }

end:
    return end_marks();
}
