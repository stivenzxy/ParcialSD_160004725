#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stddef.h>

void trim_str(char *s);
void get_value(const char *src, const char *key, char *dest, size_t dest_size);

#endif