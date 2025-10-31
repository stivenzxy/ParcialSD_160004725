#include <string.h>
#include <ctype.h>
#include "../include/string_utils.h"

void trim_str(char *s)
{
    char *p = s;
    while (*p && isspace((unsigned char)*p))
        p++;
    if (p != s)
        memmove(s, p, strlen(p) + 1);
    int len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1]))
        s[--len] = '\0';
}

void get_value(const char *src, const char *key, char *dest, size_t dest_size)
{
    char *pos = strstr(src, key);
    if (!pos)
    {
        dest[0] = '\0';
        return;
    }
    pos += strlen(key);
    char *end = strchr(pos, '|');
    size_t len = end ? (size_t)(end - pos) : strlen(pos);
    if (len >= dest_size)
        len = dest_size - 1;
    strncpy(dest, pos, len);
    dest[len] = '\0';
}
