#ifndef SEXP_H
#define SEXP_H

#include <stdbool.h>

#include "token.h"

typedef long num_t;
typedef struct cons cons_t;
typedef struct sexp sexp_t;

enum sexp_type { SEXP_NULL, SEXP_SYMBOL, SEXP_NUMBER, SEXP_CONS };
struct cons { sexp_t *car, *cdr; };

struct sexp {
    enum sexp_type type; // type of sexp: null, symbol, or cons
    union {
        tok_t *symbol; // token if type is symbol or null
        num_t *number; // number if type is number
        cons_t *cons;  // cons if type is cons
    };
};

typedef enum sexp_type sexp_type_t;

extern sexp_t *sexp_null;
extern sexp_t *sexp_quote;

num_t *make_number(const tok_t *token);
void dest_number(num_t *number);

cons_t *make_cons(sexp_t *car, sexp_t *cdr);
void dest_cons(cons_t *cons);

sexp_t *make_sexp(sexp_type_t type, tok_t *symbol, num_t *number,
                  sexp_t *car, sexp_t *cdr);
sexp_t *make_sexp_symbol(tok_t *symbol);
sexp_t *make_sexp_number(tok_t *number);
sexp_t *make_sexp_cons(sexp_t *car, sexp_t *cdr);
void dest_sexp(sexp_t *sexp);

bool sexp_is_null(const sexp_t *sexp);
bool sexp_is_symbol(const sexp_t *sexp);
bool sexp_is_cons(const sexp_t *sexp);
bool sexp_is_eq(const sexp_t *sexp1, const sexp_t *sexp2);

tok_t *sexp_symbol(const sexp_t *sexp);
sexp_t *sexp_symbol_set(sexp_t *sexp, tok_t *symbol);
num_t *sexp_number(const sexp_t *sexp);
sexp_t *sexp_number_set(sexp_t *sexp, num_t *number);
sexp_t *sexp_car(sexp_t *sexp);
sexp_t *sexp_cdr(sexp_t *sexp);
sexp_t *sexp_car_set(sexp_t *sexp, sexp_t *car);
sexp_t *sexp_cdr_set(sexp_t *sexp, sexp_t *cdr);
sexp_t *sexp_cons(sexp_t *car, sexp_t *cdr);
sexp_t *sexp_assq(sexp_t *alst, const sexp_t *symbol);
sexp_t *sexp_reverse(sexp_t *sexp);

void print_sexp(sexp_t *sexp);

#endif // SEXP_H
