#ifndef READER_H
#define READER_H

#include <stdio.h>

#include "vector.h"

char *read_word(FILE *file);
vec_t *read_sexp(FILE *file);

#endif // READER_H
