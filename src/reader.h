// reader.h - S-expression reader

#ifndef READER_H
#define READER_H

#include <stdio.h>

#include "sexp.h"
#include "types.h"

sexp_t *crow_read(FILE *file);

#endif // READER_H
