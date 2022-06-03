// string.h - string utility functions

#ifndef STRING_H
#define STRING_H

#include <stddef.h>

char *str_sub(const char *str, size_t start, size_t end);
char *str_tok(const char *str, const char *del_drop, const char *del_keep);

#endif // STRING_H
