#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>

/* fops.c for COMP3301 A2.
 * Written by Sam Kingston, 2010-2011.
 * Version 2011/1.0.
 */

#include "ioctl.h"

static int device_open(struct inode *inode, struct file *filp)
{
    try_module_get(THIS_MODULE); /* increase the refcount of the open module */
    return 0;
}

static int device_release(struct inode *inode, struct file *filp)
{
    module_put(THIS_MODULE); /* decrease the refcount of the open module */
    return 0;
}

static ssize_t device_read(struct file *filp, char *buf, size_t len,
        loff_t * off)
{
    return -ENOSYS;
}

static ssize_t device_write(struct file *filp, const char *buf, size_t len, 
        loff_t * off)
{
    return -ENOSYS;
}

static int device_ioctl(struct inode *inode, struct file *filp, 
        unsigned int cmd, unsigned long arg)
{
    return -ENOTTY;
}

static int device_mmap(struct file *filp, struct vm_area_struct *vma)
{
    return -ENOSYS;
}

struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
    .ioctl = device_ioctl,
    .mmap = device_mmap
};
