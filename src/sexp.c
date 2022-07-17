#include "sexp.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "token.h"
#include "types.h"

#define _tok_null  &(tok_t){ TOK_NULL,  "()" }
#define _tok_quote &(tok_t){ TOK_QUOTE, "quote" }
#define _tok_begin &(tok_t){ TOK_QUOTE, "begin" }

sexp_t *sexp_null  = &(sexp_t){ SEXP_NULL,   { _tok_null  } };
sexp_t *sexp_quote = &(sexp_t){ SEXP_SYMBOL, { _tok_quote } };
sexp_t *sexp_begin = &(sexp_t){ SEXP_SYMBOL, { _tok_begin } };

num_t *make_number(const tok_t *token) {
    char *endptr;
    num_t value = strtol(token->word, &endptr, 10);
    if (*endptr) error("make_number: invalid number token: %s", token->word);

    num_t *number = malloc(sizeof(num_t));
    if (!number) error("make_number: failed to allocate number");
    *number = value;

    return number;
}

void dest_number(num_t *number) {
    free(number);
}

cons_t *make_cons(sexp_t *car, sexp_t *cdr) {
    cons_t *cons = malloc(sizeof(cons_t));
    if (!cons) error("make_cons: failed to allocate cons");

    cons->car = car;
    cons->cdr = cdr;

    return cons;
}

void dest_cons(cons_t *cons) {
    dest_sexp(cons->car);
    dest_sexp(cons->cdr);
    free(cons);
}

sexp_t *make_sexp(sexp_type_t type, tok_t *symbol, num_t *number,
                  sexp_t *car, sexp_t *cdr)
{
    sexp_t *sexp = calloc(1, sizeof(sexp_t));
    if (!sexp) error("make_sexp: failed to allocate sexp");

    sexp->type = type;
    switch (type) {
    case SEXP_NULL: case SEXP_SYMBOL: sexp_symbol_set(sexp, symbol); break;
    case SEXP_NUMBER: sexp_number_set(sexp, number); break;
    case SEXP_CONS:
        sexp->cons = make_cons(NULL, NULL);
        sexp_car_set(sexp, car);
        sexp_cdr_set(sexp, cdr);
        break;
    default: error("make_sexp: invalid type: %d", type);
    }

    return sexp;
}

sexp_t *make_sexp_symbol(tok_t *symbol) {
    assert(symbol->type == TOK_SYMBOL);
    return make_sexp(SEXP_SYMBOL, symbol, NULL, NULL, NULL);
}

sexp_t *make_sexp_number(tok_t *token) {
    assert(token->type == TOK_NUMBER);
    num_t *number = make_number(token);
    return make_sexp(SEXP_NUMBER, NULL, number, NULL, NULL);
}

sexp_t *make_sexp_cons(sexp_t *car, sexp_t *cdr) {
    return make_sexp(SEXP_CONS, NULL, NULL, car, cdr);
}

void dest_sexp(sexp_t *sexp) {
    if (sexp) {
        if (sexp_is_symbol(sexp)) {
            dest_tok(sexp_symbol(sexp));
        } else {
            dest_sexp(sexp_car(sexp));
            dest_sexp(sexp_cdr(sexp));
        }
        free(sexp);
    }
}

sexp_t *sexp_closure(sexp_t *sexp, sexp_t *env) {
    assert(sexp); assert(env);
    return make_closure(sexp_car(sexp),
                        sexp_cons(sexp_begin, sexp_cdr(sexp)),
                        env);
}

sexp_type_t sexp_type(const sexp_t *sexp) {
    return sexp->type;
}

bool sexp_is_null(const sexp_t *sexp) {
    return sexp->type == SEXP_NULL;
}

bool sexp_is_symbol(const sexp_t *sexp) {
    return sexp->type == SEXP_SYMBOL;
}

bool sexp_is_number(const sexp_t *sexp) {
    return sexp->type == SEXP_NUMBER;
}

bool sexp_is_cons(const sexp_t *sexp) {
    return sexp->type == SEXP_CONS;
}

bool sexp_is_primitive(const sexp_t *sexp) {

}

bool sexp_is_closure(const sexp_t *sexp) {

}

bool sexp_is_eq(const sexp_t *sexp1, const sexp_t *sexp2) {
    assert(sexp_is_symbol(sexp1)); assert(sexp_is_symbol(sexp2));
    return strcmp(sexp_symbol(sexp1)->word, sexp_symbol(sexp2)->word) == 0;
}

tok_t *sexp_symbol(const sexp_t *s) {
    return (s && s->type == SEXP_SYMBOL) ? s->symbol : NULL;
}

sexp_t *sexp_symbol_set(sexp_t *s, tok_t *symbol) {
    if (!s || !symbol || s->type != SEXP_SYMBOL) return NULL;
    s->symbol = symbol;
    return s;
}

num_t *sexp_number(const sexp_t *s) {
    return (s && s->type == SEXP_NUMBER) ? s->number : NULL;
}

sexp_t *sexp_number_set(sexp_t *s, num_t *number) {
    if (!s || !number || s->type != SEXP_NUMBER) return NULL;
    s->number = number;
    return s;
}

#define SEXP_CXR(X)                                     \
    sexp_t *sexp_ ## X(const sexp_t *s) {               \
        if (!s || s->type != SEXP_CONS)                 \
            return NULL;                                \
        return s->cons->X;                              \
    }                                                   \
                                                        \
    sexp_t *sexp_ ## X ## _set(sexp_t *s, sexp_t *e) {  \
        if (!s || !e || s->type != SEXP_CONS)           \
            return NULL;                                \
        s->cons->X = e;                                 \
        return s;                                       \
    }                                                   \

#define SEXP_CXXR(X, ...)                           \
    sexp_t *sexp_ ## X(const sexp_t *s) {           \
        sexp_t *(*args[])(const sexp_t *s) =        \
            { __VA_ARGS__ __VA_OPT__(,) NULL };     \
        sexp_t *w = (*args[0])(s);                  \
        for (size_t i = 1; args[i] != NULL; ++i) {  \
            w = (*args[i])(w);                      \
        }                                           \
        return w;                                   \
    }                                               \

#define SEXP_CXXR_SET(X, SET, ...)                      \
    sexp_t *sexp_ ## X ## _set(sexp_t *s, sexp_t *e) {  \
        sexp_t *(*args[])(const sexp_t *s) =            \
            { __VA_ARGS__ __VA_OPT__(,) NULL };         \
        sexp_t *w = s;                                  \
        for (size_t i = 0; args[i] != NULL; ++i) {      \
            w = (*args[i])(w);                          \
        }                                               \
        SET(w, e);                                      \
        return w;                                       \
    }                                                   \

SEXP_CXR(car)
SEXP_CXR(cdr)
SEXP_CXXR(caar, sexp_car, sexp_car)
SEXP_CXXR(cadr, sexp_cdr, sexp_car)
SEXP_CXXR(cdar, sexp_car, sexp_cdr)
SEXP_CXXR(cddr, sexp_cdr, sexp_cdr)
SEXP_CXXR(caaar, sexp_car, sexp_car, sexp_car)
SEXP_CXXR(caadr, sexp_cdr, sexp_car, sexp_car)
SEXP_CXXR(cadar, sexp_car, sexp_cdr, sexp_car)
SEXP_CXXR(caddr, sexp_cdr, sexp_cdr, sexp_car)
SEXP_CXXR(cdaar, sexp_car, sexp_car, sexp_cdr)
SEXP_CXXR(cdadr, sexp_cdr, sexp_car, sexp_cdr)
SEXP_CXXR(cddar, sexp_car, sexp_cdr, sexp_cdr)
SEXP_CXXR(cdddr, sexp_cdr, sexp_cdr, sexp_cdr)
SEXP_CXXR_SET(caar, sexp_car_set, sexp_car)
SEXP_CXXR_SET(cadr, sexp_car_set, sexp_cdr)
SEXP_CXXR_SET(cdar, sexp_cdr_set, sexp_car)
SEXP_CXXR_SET(cddr, sexp_cdr_set, sexp_cdr)
SEXP_CXXR_SET(caaar, sexp_car_set, sexp_car, sexp_car)
SEXP_CXXR_SET(caadr, sexp_car_set, sexp_cdr, sexp_car)
SEXP_CXXR_SET(cadar, sexp_car_set, sexp_car, sexp_cdr)
SEXP_CXXR_SET(caddr, sexp_car_set, sexp_cdr, sexp_cdr)
SEXP_CXXR_SET(cdaar, sexp_cdr_set, sexp_car, sexp_car)
SEXP_CXXR_SET(cdadr, sexp_cdr_set, sexp_cdr, sexp_car)
SEXP_CXXR_SET(cddar, sexp_cdr_set, sexp_car, sexp_cdr)
SEXP_CXXR_SET(cdddr, sexp_cdr_set, sexp_cdr, sexp_cdr)

#undef SEXP_CXR
#undef SEXP_CXXR
#undef SEXP_CXXR_SET

sexp_t *sexp_cons(sexp_t *car, sexp_t *cdr) {
    return make_sexp_cons(car, cdr);
}

sexp_t *sexp_assq(sexp_t *alst, const sexp_t *symbol) {
    for (sexp_t *lst = alst; !sexp_is_null(lst); lst = sexp_cdr(lst)) {
        sexp_t *pair = sexp_car(lst);
        if (sexp_is_eq(sexp_car(pair), symbol))
            return pair;
    }
    return sexp_null;
}

sexp_t *sexp_reverse(sexp_t *sexp) {
    sexp_t *prev = NULL;
    for (sexp_t *walk = sexp, *next = NULL; walk != NULL;) {
        next = sexp_cdr(walk);
        sexp_cdr_set(walk, prev);
        prev = walk;
        walk = next;
    }
    return prev;
}

void print_sexp(sexp_t *sexp) {
    if (sexp_is_symbol(sexp)) {
        fputs(sexp->symbol->word, stdout);
    } else if (sexp_is_number(sexp)) {
        printf("%ld", *(sexp->number));
    } else {
        putchar('(');
        for (sexp_t *walk = sexp; !sexp_is_null(walk); walk = sexp_cdr(walk)) {
            print_sexp(sexp_car(walk));
            if (!sexp_is_null(sexp_cdr(walk))) putchar(' ');
        }
        putchar(')');
    }
}
