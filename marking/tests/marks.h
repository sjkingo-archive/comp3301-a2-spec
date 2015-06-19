#ifndef MARKS_H
#define MARKS_H

/* Flag to signify whether or not the device needs to be reloaded after
 * testing.
 */
extern bool device_dirty;

void reset_marks(unsigned int total);
void update_marks(unsigned int by);
unsigned int get_marks(void);
unsigned int end_marks(void);
void manual_test(unsigned int total);

#endif
