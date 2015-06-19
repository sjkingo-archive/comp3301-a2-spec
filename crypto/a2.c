#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h> /* kmalloc() and kfree() */
#include <linux/vmalloc.h>
#include <asm/uaccess.h> /* get_user() */

#include "buffer.h"
#include "ioctl.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sam Kingston <sam@sjkwi.com.au>");
MODULE_DESCRIPTION("Encrypting device driver");

#define DEVICE_NAME "crypto"

/* Major number for the registered device, as returned by register_chrdev() */
static int major;

static int device_open(struct inode *inode, struct file *filp)
{
    try_module_get(THIS_MODULE);
    filp->private_data = kmalloc(sizeof(struct file_state), GFP_KERNEL);
    STATE(filp)->attached_to = 0;
    printk(KERN_INFO "device opened\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *filp)
{
    int id;

    id = STATE(filp)->attached_to;
    if (id != 0) {
	detach_from_buffer(STATE(filp));
    }

    kfree(STATE(filp));
    printk(KERN_INFO "device closed\n");

    module_put(THIS_MODULE);
    return 0;
}

static ssize_t device_read(struct file *filp, char *buf, size_t len,
        loff_t * off)
{
    printk(KERN_INFO "Requested read for %ld bytes\n", (long)len);
    if (STATE(filp)->attached_to == 0) {
        printk(KERN_INFO "Requested a read when not attached to a buffer\n");
        return -EINVAL;
    }
    return -ENOSYS;
}

static ssize_t device_write(struct file *filp, const char *buf, size_t len, 
        loff_t * off)
{
    unsigned int id, space_left;
    int actual;
    char *kbuf;

    printk(KERN_INFO "Requested write for %ld bytes\n", (long)len);

    /* make sure the user has attached to a buffer first */
    id = STATE(filp)->attached_to;
    if (id == 0) {
        printk(KERN_INFO "Requested a write when not attached to a buffer\n");
        return -EINVAL;
    }

    /* we can't write more than the buffer size, so just bail out now */
    if (len > BUF_SIZE) {
	printk(KERN_INFO "Requested write for more than buffer size!\n");
	return -ENOBUFS;
    }

    /* see if there is enough space in the buffer to fit this write */
    space_left = check_buffer_space(id);
    if (len > space_left) {
	printk(KERN_INFO "Requested write for %ld bytes but only %d bytes are "
		"free in the buffer", (long)len, space_left);
	return -ENOBUFS;
    }

    /* allocate enough space in kernel memory (yuk!) and copy from user-space */
    kbuf = vmalloc(len);
    if (copy_from_user(kbuf, buf, len) != 0) {
	return -EFAULT;
    }

    actual = write_to_buffer(id, kbuf, len);
    vfree(kbuf);

    return actual;

#if 0
    while (bytes > 0) {
        char *kbuf = kmalloc(1024, GFP_KERNEL);
        ssize_t b;
        int r;
        
        if ((r = copy_from_user(kbuf, buf, sizeof(kbuf))) != 0) {
            return -EFAULT;
        }
        bytes -= sizeof(kbuf);

        b = write_to_buffer(id, buf, sizeof(kbuf));
        printk(KERN_INFO "Wrote %ld bytes to buffer %ld\n", (long)b, id);
        kfree(kbuf);
    }
#endif
}

static int handle_smode(unsigned long ptr)
{
    struct crypto_smode m;
    if (copy_from_user(&m, (void *)ptr, sizeof(m)) != 0) {
        return -EFAULT;
    }
    printk(KERN_INFO "smode: dir %d, mode %d, key %x\n", m.dir, m.mode, m.key);
    return 0;
}

static int device_ioctl(struct inode *inode, struct file *filp, 
        unsigned int cmd, unsigned long arg)
{
    int retval = -ENOTTY;
    int attach_r;

    printk(KERN_INFO "Requested ioctl for command %d\n", cmd);

    switch (cmd) {
        case CRYPTO_IOCCREATE:
            retval = create_new_buffer();
	    attach_r = attach_to_buffer(retval, STATE(filp));
	    if (attach_r != 0) {
		retval = attach_r;
	    }
            break;
        case CRYPTO_IOCTDELETE:
            retval = delete_buffer(arg);
            break;
        case CRYPTO_IOCTATTACH:
            retval = attach_to_buffer(arg, STATE(filp));
            break;
        case CRYPTO_IOCDETACH:
            retval = detach_from_buffer(STATE(filp));
            break;
        case CRYPTO_IOCSMODE:
            retval = handle_smode(arg);
            break;
    }

    return retval;
}

static struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
    .ioctl = device_ioctl
};

int __init init_module(void)
{
    if ((major = register_chrdev(0, DEVICE_NAME, &fops)) < 0) {
        printk(KERN_ALERT "Failed to register_chrdev(): error %d\n", major);
        return major;
    }

    printk(KERN_INFO "Hello from %s with major %d\n", DEVICE_NAME, major);
    printk(KERN_INFO "  mknod /dev/%s c %d 0\n", DEVICE_NAME, major);
    printk(KERN_INFO "%d buffers already exist\n", list_size());

    return 0;
}

void __exit cleanup_module(void)
{
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Unregistered %s\n", DEVICE_NAME);
}
