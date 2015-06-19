#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h> /* kmalloc() and kfree() */
#include <linux/vmalloc.h> /* vmalloc() and vfree() */
#include <linux/list.h>

#include "buffer.h"

struct list_head buffer_list;
LIST_HEAD(buffer_list);
static unsigned int next_id = 1;

static struct list_head *get_buffer_ptr(unsigned int id)
{
    struct list_head *ptr;
    struct crypto_buffer *entry;
    list_for_each(ptr, &buffer_list) {
        entry = list_entry(ptr, struct crypto_buffer, list);
        if (entry->id == id) {
            return ptr;
        }
    }
    return NULL;
}

unsigned int list_size(void)
{
    unsigned int n = 0;
    struct list_head *ptr;
    list_for_each(ptr, &buffer_list) {
        n++;
    }
    return n;
}

int create_new_buffer(void)
{
    struct crypto_buffer *new = kmalloc(sizeof(*new), GFP_KERNEL);
    if (new == NULL) {
	printk(KERN_WARNING "kmalloc() failed requested %ld bytes for struct\n", 
		sizeof(*new));
	return -1;
    }
    memset(new, 0, sizeof(*new));
    new->id = next_id++;
    new->buf = vmalloc(BUF_SIZE);
    if (new->buf == NULL) {
	printk(KERN_WARNING "vmalloc() failed requesting 8K for buffer\n");
	return -1;
    }
    memset(new->buf, 0, sizeof(new->buf));
    list_add_tail(&new->list, &buffer_list);
    return new->id;
}

int delete_buffer(unsigned int id)
{
    struct list_head *ptr;
    struct crypto_buffer *b;
    
    if ((b = get_buffer(id)) == NULL) {
        return -EINVAL;
    }

    if (b->refcount != 0) {
        printk(KERN_INFO "Buffer %d still has positive refcount (%d) and "
                "cannot be deleted\n", id, b->refcount);
        return -EINVAL;
    }

    vfree(b->buf); /* virtual address! */

    ptr = get_buffer_ptr(id);
    list_del(ptr);

    kfree(b); /* physical address! */

    return 0;
}

struct crypto_buffer *get_buffer(unsigned int id)
{
    struct list_head *ptr = get_buffer_ptr(id);
    if (ptr == NULL) {
        return NULL;
    } else {
        return list_entry(ptr, struct crypto_buffer, list);
    }
}

int attach_to_buffer(unsigned int id, struct file_state *state)
{
    struct list_head *ptr;
    struct crypto_buffer *entry;

    if (state->attached_to != 0) {
        printk(KERN_INFO "Already attached to a buffer\n");
        return -EINVAL;
    }

    list_for_each(ptr, &buffer_list) {
        entry = list_entry(ptr, struct crypto_buffer, list);
        if (entry->id == id) {
            entry->refcount++;
            state->attached_to = id;
            printk(KERN_INFO "Attached to buffer %d\n", id);
            return 0;
        }
    }

    return -EINVAL;
}

int detach_from_buffer(struct file_state *state)
{
    struct crypto_buffer *b;

    if (state->attached_to == 0) {
        printk(KERN_INFO "No buffer to detach from\n");
        return -EINVAL;
    }

    b = get_buffer(state->attached_to);
    b->refcount--;

    printk(KERN_INFO "Detached from buffer %d\n", state->attached_to);
    state->attached_to = 0;
    return 0;
}

unsigned int check_buffer_space(unsigned int id)
{
    struct crypto_buffer *b = get_buffer(id);
    return ((BUF_SIZE - b->pos_w) + b->pos_r);
}

ssize_t write_to_buffer(unsigned int id, const char *data, size_t len)
{
    /* this should only be called once it is verified that there is enough
     * space in the buffer, as it won't do any checking and instead will
     * just blindly write to it!
     */

    struct crypto_buffer *b = get_buffer(id);
    unsigned int old_pos_w, bytes_at_end, remainder, bytes;

    old_pos_w = b->pos_w;
    printk(KERN_INFO "writing to buffer at: pos %d, ptr %p\n", old_pos_w, b->buf);

    /* work out how much data can fit at the end of the buffer */
    bytes_at_end = BUF_SIZE - b->pos_w;
    if (bytes_at_end > len) {
	bytes = len;
    } else {
	bytes = bytes_at_end;
    }
    printk(KERN_INFO "can fit %d bytes of %d, starting at offset %d\n", bytes_at_end, bytes, b->pos_w);

    /* copy the data to the end of the buffer */
    memcpy(b->buf + b->pos_w, data, bytes);
    b->pos_w += bytes;
    
    /* if there is still some data left over, copy it to the start of the buffer */
    remainder = len - bytes;
    printk(KERN_INFO "can fit remaining %d bytes at start of buffer\n", remainder);
    if (remainder > 0) {
	memcpy(b->buf, data + bytes, remainder);
	bytes += remainder;
	b->pos_w = 0;
    } else {
	printk(KERN_INFO "not copying anything to start\n");
    }

    if (bytes != len) {
	printk(KERN_WARNING "bug in write_to_buffer(): only copied %d bytes of "
		"%d possible\n", bytes, (int)len);
    }

    printk(KERN_INFO "Wrote %ld bytes to buffer starting at pos %d\n", (long)len,
	    old_pos_w);
    printk(KERN_INFO "(new pos_w = %d)\n", b->pos_w);
    printk(KERN_INFO "(buffer contents: \"%s\"\n", b->buf);

    return len;
}
