#include "string.h"

#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include "error.h"

bool str_is_numeric(const char *str) {
    char *endptr;
    strtol(str, &endptr, 10);
    return *endptr == '\0';
}

char *str_sub(const char *str, size_t start, size_t end) {
    if (!str || start > end) return NULL;

    const size_t len = end - start;
    char *dst = malloc(len + 1);
    if (!dst) error("substr: malloc failed");

    memcpy(dst, str + start, len);
    dst[len] = '\0';

    return dst;
}

char *str_tok(const char *str, const char *del_drop, const char *del_keep) {
    static char *dup;   // duplicate of str, unless str is NULL
    static size_t len;  // length of str currently contained in dup
    static size_t next; // index of next character to extract from dup

    // If str is not null, make a duplicate of str, and set next to zero,
    // meaning start extracting at the beginning of the string.
    if (str) {
        dup = strdup(str);
        if (!dup) error("str_tok: strdup failed");
        len = strlen(str);
        next = 0;
    }

    // Extract and return the next token from the string, or return NULL if
    // there are no remaining tokens.
    bool found = false;  // found a token to extract
    size_t start = next; // start of substring
    for (; next < len; ++next) {
        const char c = dup[next];
        if (c == '\0') {
            break;
        } else if (strchr(del_drop, c)) {
            if (found) break;
        } else if (strchr(del_keep, c)) {
            if (!found) {
                found = true;
                start = next;
                ++next;
            }
            break;
        } else {
            if (!found) {
                found = true;
                start = next;
            }
        }
    }
    return found ? str_sub(dup, start, next) : NULL;
}
