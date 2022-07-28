#ifndef SEXP_H
#define SEXP_H

#include <stdbool.h>

#include "closure.h"
#include "token.h"
#include "types.h"

enum sexp_type {
    SEXP_NULL, SEXP_SYMBOL, SEXP_NUM, SEXP_CONS,
    SEXP_CLOSURE, SEXP_PRIMITIVE
};

enum num_type {
    NUM_INTEGER, NUM_FLOATING
};

struct num {
    enum num_type type;
    union {
        num_integer_t integer;
        num_floating_t floating;
    };
};

struct cons { sexp_t *car, *cdr; };

struct sexp {
    enum sexp_type type;
    union {
        tok_t *symbol;
        num_t *num;
        cons_t *cons;
        sexp_t *(*func)(sexp_t *);
    };
};

extern sexp_t *sexp_null;
extern sexp_t *sexp_quote;
extern sexp_t *sexp_begin;

num_t *make_num(void);
num_t *make_num_integer(num_integer_t val);
num_t *make_num_floating(num_floating_t val);
void dest_num(num_t *num);
num_t *tok2num(const tok_t *token);

cons_t *make_cons(sexp_t *car, sexp_t *cdr);
void dest_cons(cons_t *cons);

sexp_t *make_sexp(sexp_type_t type, tok_t *symbol, num_t *num,
                  sexp_t *car, sexp_t *cdr, sexp_t *(*func)(sexp_t *));
sexp_t *make_sexp_symbol(tok_t *symbol);
sexp_t *make_sexp_num(num_t *num);
sexp_t *make_sexp_cons(sexp_t *car, sexp_t *cdr);
sexp_t *make_sexp_primitive(sexp_t *(*func)(sexp_t *));
void dest_sexp(sexp_t *sexp);

sexp_t *sexp_closure(sexp_t *sexp, sexp_t *env);
sexp_t *sexp_primitive(sexp_t *(*func)(sexp_t *));

sexp_type_t sexp_type(const sexp_t *sexp);
sexp_t *sexp_type_set(sexp_t *sexp, sexp_type_t type);
bool sexp_is_null(const sexp_t *sexp);
bool sexp_is_symbol(const sexp_t *sexp);
bool sexp_is_num(const sexp_t *sexp);
bool sexp_is_cons(const sexp_t *sexp);
bool sexp_is_closure(const sexp_t *sexp);
bool sexp_is_primitive(const sexp_t *sexp);
bool sexp_is_list(const sexp_t *s);

bool sexp_is_eq(const sexp_t *sexp1, const sexp_t *sexp2);

tok_t *sexp_symbol(const sexp_t *sexp);
sexp_t *sexp_symbol_set(sexp_t *sexp, tok_t *symbol);
num_t *sexp_num(const sexp_t *sexp);
sexp_t *sexp_num_set(sexp_t *sexp, num_t *num);

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
