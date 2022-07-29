#include "sexp.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "token.h"
#include "types.h"

// extern ----------------------------------------------------------------------

#define _tok_null   &(tok_t){ TOK_NULL,   "()" }
#define _tok_quote  &(tok_t){ TOK_QUOTE,  "quote" }
#define _tok_lambda &(tok_t){ TOK_QUOTE,  "lambda" }
#define _tok_begin  &(tok_t){ TOK_SYMBOL, "begin" }
#define _tok_t_sym  &(tok_t){ TOK_SYMBOL, "t" }
#define _tok_else   &(tok_t){ TOK_SYMBOL, "else" }

sexp_t *sexp_null   = &(sexp_t){ SEXP_NULL,   { _tok_null  } };
sexp_t *sexp_quote  = &(sexp_t){ SEXP_SYMBOL, { _tok_quote } };
sexp_t *sexp_lambda = &(sexp_t){ SEXP_SYMBOL, { _tok_lambda } };
sexp_t *sexp_begin  = &(sexp_t){ SEXP_SYMBOL, { _tok_begin } };
sexp_t *sexp_t_sym  = &(sexp_t){ SEXP_SYMBOL, { _tok_t_sym } };
sexp_t *sexp_else   = &(sexp_t){ SEXP_SYMBOL, { _tok_else } };

// num -------------------------------------------------------------------------

num_t *make_num(void) {
    num_t *num = malloc(sizeof(num_t));
    if (!num) error("make_num: failed to allocate num");
    return num;
}

num_t *make_num_integer(num_integer_t val) {
    num_t *num = make_num();
    num->type = NUM_INTEGER;
    num->integer = val;
    return num;
}

num_t *make_num_floating(num_floating_t val) {
    num_t *num = make_num();
    num->type = NUM_FLOATING;
    num->floating = val;
    return num;
}

void dest_num(num_t *num) {
    free(num);
}

// TODO: Add floating point number parsing.
num_t *tok2num(const tok_t *token) {
    char *endptr;
    num_integer_t val = strtol(token->word, &endptr, 10);
    if (*endptr) error("make_num: invalid num token: %s", token->word);
    return make_num_integer(val);
}

// cons ------------------------------------------------------------------------

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

// sexp ------------------------------------------------------------------------

sexp_t *make_sexp(sexp_type_t type, tok_t *symbol, num_t *num,
                  sexp_t *car, sexp_t *cdr, sexp_t *(*func)(sexp_t *))
{
    sexp_t *sexp = calloc(1, sizeof(sexp_t));
    if (!sexp) error("make_sexp: failed to allocate sexp");

    sexp->type = type;
    switch (type) {
    case SEXP_NULL: case SEXP_SYMBOL: sexp_symbol_set(sexp, symbol); break;
    case SEXP_NUM: sexp_num_set(sexp, num); break;
    case SEXP_CONS:
        sexp->cons = make_cons(NULL, NULL);
        sexp_car_set(sexp, car); sexp_cdr_set(sexp, cdr);
        break;
    case SEXP_PRIMITIVE: sexp->func = func; break;
    default: error("make_sexp: invalid type: %d", type);
    }

    return sexp;
}

sexp_t *make_sexp_symbol(tok_t *symbol) {
    assert(symbol->type == TOK_SYMBOL);
    return make_sexp(SEXP_SYMBOL, symbol, NULL, NULL, NULL, NULL);
}

sexp_t *make_sexp_num(num_t *num) {
    return make_sexp(SEXP_NUM, NULL, num, NULL, NULL, NULL);
}

sexp_t *make_sexp_cons(sexp_t *car, sexp_t *cdr) {
    return make_sexp(SEXP_CONS, NULL, NULL, car, cdr, NULL);
}

sexp_t *make_sexp_primitive(sexp_t *(*func)(sexp_t *)) {
    return make_sexp(SEXP_PRIMITIVE, NULL, NULL, NULL, NULL, func);
}

void dest_sexp(sexp_t *sexp) {
    assert(sexp);
    if (sexp_is_symbol(sexp)) {
        dest_tok(sexp_symbol(sexp));
    } else {
        dest_sexp(sexp_car(sexp));
        dest_sexp(sexp_cdr(sexp));
    }
    free(sexp);
}

sexp_t *sexp_closure(sexp_t *sexp, sexp_t *env) {
    assert(sexp); assert(sexp_is_cons(sexp));
    assert(env); assert(sexp_is_cons(env));
    sexp_t *body = sexp_cons(sexp_begin, sexp_cdr(sexp));
    return make_closure(sexp_car(sexp), body, env);
}

sexp_t *sexp_primitive(sexp_t *(*func)(sexp_t *)) {
    assert(func);
    return make_sexp_primitive(func);
}

sexp_type_t sexp_type(const sexp_t *s) {
    assert(s); return s->type; }
sexp_t *sexp_type_set(sexp_t *s, sexp_type_t type) {
    assert(s); s->type = type; return s; }

#define SEXP_IS_TYPE(NAME, TYPE)                \
    bool sexp_is_ ## NAME(const sexp_t *s) {    \
        assert(s);                              \
        return s->type == SEXP_ ## TYPE;        \
    }                                           \

SEXP_IS_TYPE(null, NULL);
SEXP_IS_TYPE(symbol, SYMBOL);
SEXP_IS_TYPE(num, NUM);
SEXP_IS_TYPE(cons, CONS);
SEXP_IS_TYPE(closure, CLOSURE);
SEXP_IS_TYPE(primitive, PRIMITIVE);

#undef SEXP_IS_TYPE

// TODO: Check if the entire thing is a list.
bool sexp_is_list(const sexp_t *s) {
    assert(s);
    return s->type == SEXP_CONS || s->type == SEXP_NULL;
}

tok_t *sexp_symbol(const sexp_t *s) {
    return (s && s->type == SEXP_SYMBOL) ? s->symbol : NULL;
}

sexp_t *sexp_symbol_set(sexp_t *s, tok_t *symbol) {
    if (!s || !symbol || s->type != SEXP_SYMBOL) return NULL;
    s->symbol = symbol;
    return s;
}

num_t *sexp_num(const sexp_t *s) {
    return (s && s->type == SEXP_NUM) ? s->num : NULL;
}

sexp_t *sexp_num_set(sexp_t *s, num_t *num) {
    if (!s || !num || s->type != SEXP_NUM) return NULL;
    s->num = num;
    return s;
}

#define SEXP_CXR(X)                                     \
    sexp_t *sexp_ ## X(const sexp_t *s) {               \
        assert(s);                                      \
        return s->cons->X;                              \
    }                                                   \
                                                        \
    sexp_t *sexp_ ## X ## _set(sexp_t *s, sexp_t *e) {  \
        assert(s && e);                                 \
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

SEXP_CXR(car);
SEXP_CXR(cdr);
SEXP_CXXR(caar, sexp_car, sexp_car);
SEXP_CXXR(cadr, sexp_cdr, sexp_car);
SEXP_CXXR(cdar, sexp_car, sexp_cdr);
SEXP_CXXR(cddr, sexp_cdr, sexp_cdr);
SEXP_CXXR(caaar, sexp_car, sexp_car, sexp_car);
SEXP_CXXR(caadr, sexp_cdr, sexp_car, sexp_car);
SEXP_CXXR(cadar, sexp_car, sexp_cdr, sexp_car);
SEXP_CXXR(caddr, sexp_cdr, sexp_cdr, sexp_car);
SEXP_CXXR(cdaar, sexp_car, sexp_car, sexp_cdr);
SEXP_CXXR(cdadr, sexp_cdr, sexp_car, sexp_cdr);
SEXP_CXXR(cddar, sexp_car, sexp_cdr, sexp_cdr);
SEXP_CXXR(cdddr, sexp_cdr, sexp_cdr, sexp_cdr);
SEXP_CXXR_SET(caar, sexp_car_set, sexp_car);
SEXP_CXXR_SET(cadr, sexp_car_set, sexp_cdr);
SEXP_CXXR_SET(cdar, sexp_cdr_set, sexp_car);
SEXP_CXXR_SET(cddr, sexp_cdr_set, sexp_cdr);
SEXP_CXXR_SET(caaar, sexp_car_set, sexp_car, sexp_car);
SEXP_CXXR_SET(caadr, sexp_car_set, sexp_cdr, sexp_car);
SEXP_CXXR_SET(cadar, sexp_car_set, sexp_car, sexp_cdr);
SEXP_CXXR_SET(caddr, sexp_car_set, sexp_cdr, sexp_cdr);
SEXP_CXXR_SET(cdaar, sexp_cdr_set, sexp_car, sexp_car);
SEXP_CXXR_SET(cdadr, sexp_cdr_set, sexp_cdr, sexp_car);
SEXP_CXXR_SET(cddar, sexp_cdr_set, sexp_car, sexp_cdr);
SEXP_CXXR_SET(cdddr, sexp_cdr_set, sexp_cdr, sexp_cdr);

#undef SEXP_CXR
#undef SEXP_CXXR
#undef SEXP_CXXR_SET

bool sexp_eq_p(const sexp_t *sexp1, const sexp_t *sexp2) {
    assert(sexp1); assert(sexp2);
    if (!sexp_is_symbol(sexp1) || !sexp_is_symbol(sexp2)) return false;
    return strcmp(sexp_symbol(sexp1)->word, sexp_symbol(sexp2)->word) == 0;
}

bool sexp_true_p(const sexp_t *sexp) {
    assert(sexp);
    return !sexp_is_null(sexp);
}

bool sexp_false_p(const sexp_t *sexp) {
    assert(sexp);
    return sexp_is_null(sexp);
}

sexp_t *sexp_cons(sexp_t *car, sexp_t *cdr) {
    return make_sexp_cons(car, cdr);
}

static num_integer_t _sexp_length(const sexp_t *sexp) {
    num_integer_t length = 0;
    for (const sexp_t *walk = sexp; !sexp_is_null(walk); walk = sexp_cdr(walk))
        ++length;
    return length;
}

sexp_t *sexp_length(const sexp_t *sexp) {
    num_integer_t length = _sexp_length(sexp);
    return make_sexp_num(make_num_integer(length));
}

sexp_t *sexp_assq(sexp_t *alst, const sexp_t *symbol) {
    for (sexp_t *lst = alst; !sexp_is_null(lst); lst = sexp_cdr(lst)) {
        sexp_t *pair = sexp_car(lst);
        if (sexp_eq_p(sexp_car(pair), symbol))
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
    assert(sexp);
    if (sexp_is_symbol(sexp)) {
        fputs(sexp->symbol->word, stdout);
    } else if (sexp_is_num(sexp)) {
        switch (sexp->num->type) {
        case NUM_INTEGER: printf("%ld", sexp->num->integer); break;
        case NUM_FLOATING: printf("%f", sexp->num->floating); break;
        }
    } else if (sexp_is_null(sexp)) {
        fputs("()", stdout);
    } else if (sexp_is_cons(sexp)) {
        putchar('(');
        sexp_t *walk;
        for (walk = sexp; sexp_is_cons(walk); walk = sexp_cdr(walk)) {
            print_sexp(sexp_car(walk));
            if (!sexp_is_null(sexp_cdr(walk))) {
                putchar(' ');
                if (!sexp_is_cons(sexp_cdr(walk)))
                    fputs(". ", stdout);
            }
        }
        if (!sexp_is_null(walk))
            print_sexp(walk);
        putchar(')');
    } else if (sexp_is_closure(sexp) || sexp_is_primitive(sexp)) {
        printf("#<procedure>");
    } else error("print_sexp: don't know how to print!");
}
