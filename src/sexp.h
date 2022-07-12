#ifndef SEXP_H
#define SEXP_H

#include <stdbool.h>

#include "token.h"

typedef struct cons {
    struct sexp *car;
    struct cons *cdr;
} cons_t;

typedef struct sexp {
    tok_t *atom;       // token if atom else NULL
    struct cons *list; // cons cell if list else NULL
} sexp_t;

// cons ------------------------------------------------------------------------

cons_t *make_cons(sexp_t *car, cons_t *cdr);
void dest_cons(cons_t *cons);

sexp_t *cons_car(cons_t *cons);
cons_t *cons_cdr(cons_t *cons);
cons_t *cons_cons(sexp_t *car, cons_t *cdr);

// sexp ------------------------------------------------------------------------

sexp_t *make_sexp(tok_t *atom, cons_t *list);
void dest_sexp(sexp_t *sexp);

bool sexp_is_atom(const sexp_t *sexp);
bool sexp_is_list(const sexp_t *sexp);

sexp_t *sexp_cons(sexp_t *car, sexp_t *cdr);
sexp_t *sexp_reverse(sexp_t *sexp);

void print_sexp(sexp_t *sexp);

#endif // SEXP_H
