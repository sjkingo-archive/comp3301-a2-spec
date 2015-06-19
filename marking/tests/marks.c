#include "common.h"

static unsigned int current_mark = 0;
static unsigned int total_mark = 0;

bool device_dirty = false;

void reset_marks(unsigned int total)
{
    current_mark = 0;
    total_mark = total;
}

void update_marks(unsigned int by)
{
    current_mark += by;
    assert(current_mark <= total_mark);
}

unsigned int get_marks(void)
{
    return current_mark;
}

unsigned int end_marks(void)
{
    printf("Test finished: %d/%d marks\n", current_mark, total_mark);
    if (device_dirty) {
        printf("!! device has been marked as dirty, need to reload it !!\n");
    }

    if (current_mark == 0) {
        return ERR_FAILURE;
    } else {
        return ERR_NONE;
    }
}

void manual_test(unsigned int total)
{
    printf("!! this test requires manual marking: %d marks total\n", total);
    exit(0);
}
