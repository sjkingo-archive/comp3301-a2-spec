/* cryptodev - a pseudo-implementation of a crypto hardware device
 * Sam Kingston <s.kingston@uq.edu.au>, for comp3301-2012.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include "cryptoapi.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sam Kingston <s.kingston@uq.edu.au>");
MODULE_DESCRIPTION("A pseudo-implementation of a crypto hardware device");

int __init init_module(void) {
    printk(KERN_INFO "cryptodev starting up\n");
    return 0;
}

void __exit cleanup_module(void) {
    printk(KERN_INFO "cryptodev closing down\n");
}

static inline void swap_bytes(u8 *a, u8 *b) {
    u8 t;
    t = *a;
    *a = *b;
    *b = t;
}

void cryptodev_init(struct cryptodev_state *const state, const u8 *key, int keylen) {
    int i;
    u8 j;

    /* initialise the state buffer */
    for (i = 0; i < sizeof(state->perm); i++) {
        state->perm[i] = (u8)i;
    }
    state->index1 = 0;
    state->index2 = 0;

    /* encrypt the state buffer using the key */
    j = 0;
    for (i = 0; i < sizeof(state->perm); i++) {
        j += state->perm[i] + key[i % keylen];
        swap_bytes(&state->perm[i], &state->perm[j]);
    }
}
EXPORT_SYMBOL(cryptodev_init);

void cryptodev_docrypt(struct cryptodev_state *const state, const u8 *in, u8 *out, int buflen) {
    int i;
    u8 j;

    for (i = 0; i < buflen; i++) {
        state->index1++;
        state->index2 += state->perm[state->index1];
        swap_bytes(&state->perm[state->index1], &state->perm[state->index2]);

        /* crypt the next byte */
        j = state->perm[state->index1] + state->perm[state->index2];
        out[i] = in[i] ^ state->perm[j];
    }
}
EXPORT_SYMBOL(cryptodev_docrypt);
