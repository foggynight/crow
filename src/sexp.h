#ifndef SEXP_H
#define SEXP_H

#include <stdbool.h>

#include "token.h"

typedef struct cons cons_t;
typedef struct sexp sexp_t;

enum sexp_type { SEXP_NULL, SEXP_ATOM, SEXP_CONS };
struct cons { sexp_t *car, *cdr; };

struct sexp {
    enum sexp_type type; // type of sexp: null, atom, or cons
    union {
        tok_t *atom;  // token if type is atom or null
        cons_t *cons; // cons if type is cons
    };
};

typedef enum sexp_type sexp_type_t;

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

tok_t *sexp_atom(const sexp_t *s);
sexp_t *sexp_atom_set(sexp_t *s, tok_t *atom);
sexp_t *sexp_car(sexp_t *sexp);
sexp_t *sexp_cdr(sexp_t *sexp);
sexp_t *sexp_car_set(sexp_t *sexp, sexp_t *car);
sexp_t *sexp_cdr_set(sexp_t *sexp, sexp_t *cdr);
sexp_t *sexp_cons(sexp_t *car, sexp_t *cdr);
sexp_t *sexp_assq(sexp_t *alst, const sexp_t *symbol);
sexp_t *sexp_reverse(sexp_t *sexp);

void print_sexp(sexp_t *sexp);

#endif // SEXP_H
