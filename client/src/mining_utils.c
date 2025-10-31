#include <string.h>
#include <stdio.h>
#include <openssl/md5.h>
#include "../include/mining_utils.h"

void md5_string(const char *input, char outputBuffer[33])
{
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5((unsigned char *)input, strlen(input), hash);
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    outputBuffer[32] = 0;
}

int next_combination(char *combo, int len)
{
    const char *charset = CHARSET;
    int base = BASE;

    for (int i = len - 1; i >= 0; i--)
    {
        char *ptr = strchr(charset, combo[i]);
        if (!ptr)
            return 0;
        int index = ptr - charset;
        if (index < base - 1)
        {
            combo[i] = charset[index + 1];
            return 1;
        }
        else
        {
            combo[i] = '0';
        }
    }
    return 0;
}

int hash_meets_condition(const char *hash, int cond_type, const char *cond_value)
{
    int pattern_len = strlen(cond_value);
    int hash_len = 32;
    switch (cond_type)
    {
    case 1:
        return (strncmp(hash, cond_value, pattern_len) == 0);
    case 2:
        return (pattern_len <= hash_len &&
                strcmp(hash + hash_len - pattern_len, cond_value) == 0);
    case 3:
        return (strstr(hash, cond_value) != NULL);
    default:
        return 0;
    }
}
