#ifndef MINING_UTILS_H
#define MINING_UTILS_H

#define CHARSET "0123456789abcdefghijklmnopqrstuvwxyz"
#define BASE 36

void md5_string(const char *input, char outputBuffer[33]);
int next_combination(char *combo, int len);
int hash_meets_condition(const char *hash, int cond_type, const char *cond_value);

#endif
