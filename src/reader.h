#ifndef READER_H
#define READER_H

#include <stdio.h>

#include "vector.h"

char *read_word(FILE *file);
size_t read_sexp(FILE *file, vec_t *words);
size_t read_sexp_str(char *str, vec_t *words);

#endif // READER_H
