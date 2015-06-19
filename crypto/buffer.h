#ifndef CRYPTO_BUFFER_H
#define CRYPTO_BUFFER_H

#include <linux/list.h>

#define BUF_SIZE 8192

/* A buffer */
struct crypto_buffer {
    struct list_head list;
    unsigned int id; /* [1 .. INT_MAX-1] */
    int refcount;
    char *buf;
    int pos_r;
    int pos_w;
};

/* Set to the filp->private_data member */
struct file_state {
    unsigned int attached_to; /* 0 for none */
};
#define STATE(filp) ((struct file_state *)filp->private_data)

#define NOT_ATTACHED 0

unsigned int list_size(void);
int create_new_buffer(void);
int delete_buffer(unsigned int id);
struct crypto_buffer *get_buffer(unsigned int id);
int attach_to_buffer(unsigned int id, struct file_state *state);
int detach_from_buffer(struct file_state *state);
unsigned int check_buffer_space(unsigned int id);
ssize_t write_to_buffer(unsigned int id, const char *data, size_t len);

#endif
