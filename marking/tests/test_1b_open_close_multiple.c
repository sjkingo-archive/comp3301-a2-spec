#include "common.h"
#include "marks.h"

#include <sys/wait.h>

#define TOTAL_MARKS 2

void try_close(int fd, bool mark)
{
    /* 1 mark: close. If open() did not work, then the student gets 0 for this
     * criteria too.
     */
    if (fd < 0) {
        fprintf(stderr, "Since all opens failed, close() cannot be tested)\n");
    } else {
        if (close(fd) == 0) {
            if (mark) 
                update_marks(1);
        } else {
            printe("close()");
        }
    }
}

int main(int argc, char **argv)
{
    populate_device(argc, argv);
    reset_marks(TOTAL_MARKS);

    /* 1 mark: open. Device must be able to be opened for reading, writing and
     * reading/writing to get this mark.
     */
    int fd = open_rdrw();
    if (fd < 0) {
        try_close(fd, false);
        goto end;
    }

    fd = open_rdonly();
    if (fd < 0) {
        try_close(fd, false);
        goto end;
    }

    fd = open_wronly();
    if (fd < 0) {
        try_close(fd, false);
        goto end;
    }

    /* fork another process and try the opens again */
    char *arg0 = "./test_1a_open_close";
    printv("Now forking to run %s\n", arg0);
    int pid = fork();
    if (pid == 0) {
        /* get rid of stdout */
        int null_fd = open("/dev/null", O_WRONLY);
        if (null_fd < 0) {
            printe("open(\"/dev/null\")");
            _exit(ERR_EXEC);
        }
        if (dup2(null_fd, STDOUT_FILENO) < 0) {
            printe("dup2()");
            _exit(ERR_EXEC);
        }

        /* execute the single open/close test */
        execl(arg0, arg0, device_node, NULL);
        printe("execl()");
        _exit(ERR_EXEC);
    } else {
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            int ret = WEXITSTATUS(status);
            if (ret == 0) {
                printv("Child returned success\n");
                update_marks(1);
            } else {
                printv("Child returned non-zero status (%d)\n", ret);
            }
        }
        try_close(fd, true);
        goto end;
    }

end:
    return end_marks();
}
