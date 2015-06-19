#ifndef _CRYPTODEV_API_H
#define _CRYPTODEV_API_H

/* struct rc4_state: stores the state of encryption/decryption */
struct cryptodev_state {
    u8 perm[256];
    u8 index1;
    u8 index2;
};

/* cryptodev_init(): initialise the state for encryption/decryption.
 *   Sets the key ready for crypto functions.
 */
extern void cryptodev_init(struct cryptodev_state *const state, const u8 *key, int keylen);

/* cryptodev_docrypt: general encryption/decryption function.
 *   Encrypts or decrypts the block given with the (pre-initialised) state
 *   buffer and stores the result in out.
 */
extern void cryptodev_docrypt(struct cryptodev_state *const state, const u8 *in, u8 *out, int buflen);

#endif
