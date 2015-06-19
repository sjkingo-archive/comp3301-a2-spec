#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void assert_str(const char *str1, const char *str2) {
    assert(strlen(str1) == strlen(str2));
    for (unsigned int i = 0; i < strlen(str1); i++) {
        assert(str1[i] == str2[i]);
    }
}

int main(void)
{
    char iv = 0xFF + 1;
    char *plain = "Sam is really cool!";

    char last = 0x0;
    char *cipher = malloc(strlen(plain) + 1);
    char *de = malloc(strlen(plain) + 1);

    // encrypt - C_i = P_i XOR C_(i-1)
    for (unsigned int i = 0; i < strlen(plain); i++) {
        char key = (last == 0x0) ? iv : last;
        printf("Using encryption key %x\n", key);
        cipher[i] = plain[i] ^ key;
        last = cipher[i];
    }

    printf("---\n");
    iv = 0xFF;

    // decrypt - P_i = C_i XOR C_(i-1)
    last = 0x0;
    for (unsigned int i = 0; i < strlen(plain); i++) {
        char key = (last == 0x0) ? iv : last;
        printf("Using decryption key %x\n", key);
        de[i] = cipher[i] ^ key;
        last = cipher[i];
    }


    printf("'%s' '%s'\n", plain, de);
    assert_str(plain, de);

    free(cipher);
    free(de);
    return 0;
}
