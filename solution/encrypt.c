#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h> /* kmalloc() and kfree() */
#include <asm/uaccess.h> /* get_user() */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sam Kingston <sam@sjkwi.com.au>");
MODULE_DESCRIPTION("Encrypting device driver");

#define DEVICE_NAME "enc"
#define BUF_SIZE 1024

struct enc_data {
    char *buf;
    unsigned int bytes;
    unsigned int pos_write;
    unsigned int pos_read;

    int checksum;
};
#define ENC_DATA(filp) ((struct enc_data *)filp->private_data)

/* Declare the fops struct that we will hand to register_chrdev() */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

/* Major number for the registered device, as returned by register_chrdev() */
static int major;

int init_module(void)
{
    if ((major = register_chrdev(0, DEVICE_NAME, &fops)) < 0) {
        printk(KERN_ALERT "Failed to register_chrdev(): error %d\n", major);
        return major;
    }

    printk(KERN_INFO "Hello from COMP3301 A2 with major %d\n", major);
    printk(KERN_INFO "  mknod /dev/%s c %d 0\n", DEVICE_NAME, major);

    return 0;
}

void cleanup_module(void)
{
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Unregistered COMP3301 A2\n");
}

static int device_open(struct inode *inode, struct file *filp)
{
    try_module_get(THIS_MODULE);

    /* set up the private structure we'll be using */
    filp->private_data = kmalloc(sizeof(struct enc_data), GFP_KERNEL);
    if (!filp->private_data) {
        return -ENOMEM;
    }
    memset(filp->private_data, 0, sizeof(struct enc_data));

    /* allocate the buffer. We do this dynamically since the kernel stack
     * is quite small and we don't want to overflow it.
     */
    if (!(ENC_DATA(filp)->buf = kmalloc(BUF_SIZE, GFP_KERNEL))) {
        kfree(filp->private_data);
        return -ENOMEM;
    }
    memset(ENC_DATA(filp)->buf, 0, BUF_SIZE);

    /* misc stuff */
    ENC_DATA(filp)->checksum = 0xDEADBEEF;

    printk(KERN_INFO "device opened\n");

    return 0;
}

static int device_release(struct inode *inode, struct file *filp)
{
    module_put(THIS_MODULE);

    /* validate the checksum - this is here for no other reason than to take
     * up CPU instructions and should really be removed..
     */
    if (ENC_DATA(filp)->checksum != 0xDEADBEEF) {
        printk(KERN_ALERT "checksum invalid: 0x%X not 0xDEADBEEF\n", 
                ENC_DATA(filp)->checksum);
    }

    kfree(ENC_DATA(filp)->buf);
    kfree(filp->private_data);
    printk(KERN_INFO "device closed\n");

    return 0;
}

static ssize_t device_read(struct file *filp, char *buf, size_t len,
        loff_t * off)
{
    struct enc_data *d = ENC_DATA(filp);
    int bytes = 0;

    printk(KERN_INFO "Requested read for %d bytes\n", len);
    printk(KERN_INFO "%d bytes in buffer, next pos = %d\n", d->bytes, d->pos_read);
    
    while (len > 0) {
        if (d->bytes == 0) {
            printk(KERN_INFO "Only returned %d bytes\n", bytes);
            return bytes;
        }

        put_user(d->buf[d->pos_read], buf++);
        d->buf[d->pos_read++] = '\0';

        d->bytes--;
        len--;
        bytes++;
    }

    return bytes;
}

static ssize_t device_write(struct file *filp, const char *buf, size_t len, 
        loff_t * off)
{
    struct enc_data *d = ENC_DATA(filp);
    int i;
    for (i = 0; i < len; i++) {
        get_user(d->buf[d->pos_write++], buf + i);
        d->bytes++;
    }

    printk(KERN_INFO "Wrote %d bytes to enc buffer\n", i);
    printk(KERN_INFO "Bytes = %d, next pos = %d\n", d->bytes, d->pos_write);

    return i;
}
