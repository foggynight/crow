#ifndef SEXP_H
#define SEXP_H

#include <stdbool.h>

#include "token.h"

typedef enum sexp_type {
    SEXP_NULL, SEXP_ATOM, SEXP_CONS
} sexp_type_t;

typedef struct sexp {
    sexp_type_t type;       // type of sexp: null, atom, or cons
    tok_t *atom;            // token if type is atom or null
    struct sexp *car, *cdr; // sexps if type is cons
} sexp_t;

extern sexp_t *sexp_null;
extern sexp_t *sexp_quote;

sexp_t *make_sexp(sexp_type_t type, tok_t *atom, sexp_t *car, sexp_t *cdr);
sexp_t *make_sexp_null(void);
sexp_t *make_sexp_atom(tok_t *atom);
sexp_t *make_sexp_cons(sexp_t *car, sexp_t *cdr);
void dest_sexp(sexp_t *sexp);

bool sexp_is_null(const sexp_t *sexp);
bool sexp_is_atom(const sexp_t *sexp);
bool sexp_is_cons(const sexp_t *sexp);
bool sexp_is_eq(const sexp_t *sexp1, const sexp_t *sexp2);

sexp_t *sexp_cons(sexp_t *car, sexp_t *cdr);
sexp_t *sexp_assq(sexp_t *alst, const sexp_t *symbol);
sexp_t *sexp_reverse(sexp_t *sexp);

void print_sexp(sexp_t *sexp);

#endif // SEXP_H
