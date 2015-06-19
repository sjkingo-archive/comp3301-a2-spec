#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>

#include "../cryptodev/cryptoapi.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sam Kingston <s.kingston@uq.edu.au>");
MODULE_DESCRIPTION("testmod for cryptodev");

int __init init_module(void) {
    u8 *key = "this is a key";
    u8 *plain = "The lazy dog";
    int keylen = strlen(key);
    int plainlen = strlen(plain);
    struct cryptodev_state s;

    u8 *out = kmalloc(1024, GFP_KERNEL);
    u8 *dec = kmalloc(1024, GFP_KERNEL);

    memset(out, 0, 1024);
    memset(dec, 0, 1024);

    cryptodev_init(&s, key, keylen);
    cryptodev_docrypt(&s, plain, out, plainlen);
    cryptodev_init(&s, key, keylen);
    cryptodev_docrypt(&s, out, dec, plainlen);

    printk("plain = '%s'\n", plain);
    printk("dec   = '%s'\n", dec);

    kfree(out);
    kfree(dec);

    return 0;
}

void __exit cleanup_module(void) {
}
