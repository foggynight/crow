#ifndef SEXP_H
#define SEXP_H

#include <stdbool.h>

#include "token.h"

// cons ------------------------------------------------------------------------

typedef struct cons {
    sexp_t *car;
    sexp_t *cdr;
} cons_t;

cons_t *make_cons(sexp_t *car, sexp_t *cdr);
void dest_cons(cons_t *cons);

sexp_t *cons_car(cons_t *cons);
sexp_t *cons_cdr(cons_t *cons);

// sexp ------------------------------------------------------------------------

typedef struct sexp {
    tok_t *atom;  // token if atom else NULL
    cons_t *list; // vector of sexps if list else NULL
} sexp_t;

sexp_t *make_sexp(tok_t *atom, vec_t *children);
void dest_sexp(sexp_t *sexp);

bool sexp_is_atom(const sexp_t *sexp);
bool sexp_is_list(const sexp_t *sexp);

#endif // SEXP_H
