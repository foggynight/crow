// string.h - string utility functions

#ifndef STRING_H
#define STRING_H

#include <stddef.h>

// Make a copy of the [start, end) substring of str, returns a pointer to the
// copy on success, returns NULL on failure.
char *str_sub(const char *str, size_t start, size_t end);

// Similar to strtok of string.h, but does not modify str, instead returning
// pointers to newly allocated strings, and it has two sets of delimiters:
//
//   del_drop: Delimiters which separate tokens.
//
//   del_keep: Delimiters which also separate tokens, but are returned as their
//             own single character tokens.
char *str_tok(const char *str, const char *del_drop, const char *del_keep);

#endif // STRING_H
