#include "../include/mining_utils.h"

void index_to_str(long index, int length, char *out) {
    const char charset[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    for (int i = length - 1; i >= 0; i--) {
        out[i] = charset[index % 36];
        index /= 36;
    }
    out[length] = '\0';
}
