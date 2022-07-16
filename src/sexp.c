#include "sexp.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "token.h"

#define _tok_null  &(tok_t){ TOK_NULL,  "()" }
#define _tok_quote &(tok_t){ TOK_QUOTE, "quote" }

sexp_t *sexp_null  = &(sexp_t){ SEXP_NULL, { _tok_null  } };
sexp_t *sexp_quote = &(sexp_t){ SEXP_ATOM, { _tok_quote } };

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

sexp_t *make_sexp(sexp_type_t type, tok_t *atom, sexp_t *car, sexp_t *cdr) {
    sexp_t *sexp = calloc(1, sizeof(sexp_t));
    if (!sexp) error("make_sexp: failed to allocate sexp_t");

    sexp->type = type;
    switch (type) {
    case SEXP_NULL: case SEXP_ATOM:
        sexp_atom_set(sexp, atom);
        break;
    case SEXP_CONS:
        sexp->cons = make_cons(NULL, NULL);
        sexp_car_set(sexp, car);
        sexp_cdr_set(sexp, cdr);
        break;
    default: error("make_sexp: invalid type: %d", type);
    }

    return sexp;
}

sexp_t *make_sexp_atom(tok_t *atom) {
    return make_sexp(SEXP_ATOM, atom, NULL, NULL);
}

sexp_t *make_sexp_cons(sexp_t *car, sexp_t *cdr) {
    return make_sexp(SEXP_CONS, NULL, car, cdr);
}

void dest_sexp(sexp_t *sexp) {
    if (sexp) {
        if (sexp_is_atom(sexp)) {
            dest_tok(sexp_atom(sexp));
        } else {
            dest_sexp(sexp_car(sexp));
            dest_sexp(sexp_cdr(sexp));
        }
        free(sexp);
    }
}

bool sexp_is_null(const sexp_t *sexp) {
    return sexp->type == SEXP_NULL;
}

bool sexp_is_atom(const sexp_t *sexp) {
    return sexp->type == SEXP_ATOM || sexp->type == SEXP_NULL;
}

bool sexp_is_cons(const sexp_t *sexp) {
    return sexp->type == SEXP_CONS;
}

bool sexp_is_eq(const sexp_t *sexp1, const sexp_t *sexp2) {
    assert(sexp_is_atom(sexp1)); assert(sexp_is_atom(sexp2));
    return strcmp(sexp_atom(sexp1)->word, sexp_atom(sexp2)->word) == 0;
}

tok_t *sexp_atom(const sexp_t *s) {
    return (s && s->type == SEXP_ATOM) ? s->atom : NULL;
}

sexp_t *sexp_atom_set(sexp_t *s, tok_t *atom) {
    if (!s || !atom || s->type != SEXP_ATOM) return NULL;
    s->atom = atom;
    return s;
}

#define SEXP_CXR(X)                             \
    sexp_t *sexp_ ## X(sexp_t *s) {             \
        if (!s || s->type != SEXP_CONS)         \
            return NULL;                        \
        return s->cons->X;                      \
    }                                           \

#define SEXP_CXR_SET(X)                                 \
    sexp_t *sexp_ ## X ## _set(sexp_t *s, sexp_t *e) {  \
        if (!s || !e || s->type != SEXP_CONS)           \
            return NULL;                                \
        s->cons->X = e;                                 \
        return s;                                       \
    }                                                   \

SEXP_CXR(car)
SEXP_CXR(cdr)
SEXP_CXR_SET(car)
SEXP_CXR_SET(cdr)

#undef SEXP_CXR
#undef SEXP_CXR_SET

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
    if (sexp_is_atom(sexp)) {
        fputs(sexp->atom->word, stdout);
    } else {
        putchar('(');
        for (sexp_t *walk = sexp; !sexp_is_null(walk); walk = sexp_cdr(walk)) {
            print_sexp(sexp_car(walk));
            if (!sexp_is_null(sexp_cdr(walk))) putchar(' ');
        }
        putchar(')');
    }
}
