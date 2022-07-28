// primitive.h - CROW primitive procedures
//
// All primitive functions take a single sexp argument and return a sexp. The
// arguments of the primitive itself are contained in the sexp argument, which
// is always a list.

#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "types.h"

sexp_t *prim_frame(void);

sexp_t *prim_cons(sexp_t *args);
sexp_t *prim_car(sexp_t *args);
sexp_t *prim_cdr(sexp_t *args);

#endif // PRIMITIVE_H
