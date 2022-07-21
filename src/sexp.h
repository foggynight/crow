#ifndef SEXP_H
#define SEXP_H

#include <stdbool.h>

#include "closure.h"
#include "token.h"
#include "types.h"

enum sexp_type {
    SEXP_NULL, SEXP_SYMBOL, SEXP_NUMBER, SEXP_CONS,
    SEXP_CLOSURE, SEXP_PRIMITIVE
};
struct cons { sexp_t *car, *cdr; };

struct sexp {
    enum sexp_type type;
    union {
        tok_t *symbol;
        num_t *number;
        cons_t *cons;
    };
};

extern sexp_t *sexp_null;
extern sexp_t *sexp_quote;
extern sexp_t *sexp_begin;

num_t *make_number(const tok_t *token);
void dest_number(num_t *number);

cons_t *make_cons(sexp_t *car, sexp_t *cdr);
void dest_cons(cons_t *cons);

sexp_t *make_sexp(sexp_type_t type, tok_t *symbol, num_t *number,
                  sexp_t *car, sexp_t *cdr);
sexp_t *make_sexp_symbol(tok_t *symbol);
sexp_t *make_sexp_number(tok_t *number);
sexp_t *make_sexp_cons(sexp_t *car, sexp_t *cdr);
//sexp_t *make_sexp_primitive(...);
void dest_sexp(sexp_t *sexp);

sexp_t *sexp_closure(sexp_t *sexp, sexp_t *env);

sexp_type_t sexp_type(const sexp_t *sexp);
sexp_t *sexp_type_set(sexp_t *sexp, sexp_type_t type);
bool sexp_is_null(const sexp_t *sexp);
bool sexp_is_symbol(const sexp_t *sexp);
bool sexp_is_cons(const sexp_t *sexp);
bool sexp_is_closure(const sexp_t *sexp);
bool sexp_is_primitive(const sexp_t *sexp);
bool sexp_is_eq(const sexp_t *sexp1, const sexp_t *sexp2);

tok_t *sexp_symbol(const sexp_t *sexp);
sexp_t *sexp_symbol_set(sexp_t *sexp, tok_t *symbol);
num_t *sexp_number(const sexp_t *sexp);
sexp_t *sexp_number_set(sexp_t *sexp, num_t *number);

#define SEXP_CXR(X)                                     \
    sexp_t *sexp_ ## X(const sexp_t *s);                \
    sexp_t *sexp_ ## X ## _set(sexp_t *s, sexp_t *e);   \

SEXP_CXR(car);
SEXP_CXR(cdr);
SEXP_CXR(caar);
SEXP_CXR(cadr);
SEXP_CXR(cdar);
SEXP_CXR(cddr);
SEXP_CXR(caaar);
SEXP_CXR(caadr);
SEXP_CXR(cadar);
SEXP_CXR(caddr);
SEXP_CXR(cdaar);
SEXP_CXR(cdadr);
SEXP_CXR(cddar);
SEXP_CXR(cdddr);

#undef SEXP_CXR

sexp_t *sexp_cons(sexp_t *car, sexp_t *cdr);
sexp_t *sexp_length(const sexp_t *sexp);
sexp_t *sexp_assq(sexp_t *alst, const sexp_t *symbol);
sexp_t *sexp_reverse(sexp_t *sexp);

void print_sexp(sexp_t *sexp);

#endif // SEXP_H
