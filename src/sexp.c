#include "sexp.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "token.h"

#define tok_null  &(tok_t){ TOK_NULL,  "()" }
#define tok_quote &(tok_t){ TOK_QUOTE, "quote" }

sexp_t *sexp_null  = &(sexp_t){ SEXP_NULL, tok_null  };
sexp_t *sexp_quote = &(sexp_t){ SEXP_ATOM, tok_quote };

sexp_t *make_sexp(sexp_type_t type, tok_t *atom, sexp_t *car, sexp_t *cdr) {
    sexp_t *sexp = calloc(1, sizeof(sexp_t));
    if (!sexp) error("make_sexp: failed to allocate sexp_t");

    sexp->type = type;
    switch (type) {
    case SEXP_NULL: case SEXP_ATOM: sexp_atom_set(sexp, atom); break;
    case SEXP_CONS: sexp_car_set(sexp, car); sexp_cdr_set(sexp, cdr); break;
    default: error("make_sexp: invalid type argument: %d", type);
    }

    return sexp;
}

sexp_t *make_sexp_null(void) {
    return make_sexp(SEXP_NULL, tok_null, NULL, NULL);
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
            dest_tok(sexp->atom);
        } else {
            dest_sexp(sexp->car);
            dest_sexp(sexp->cdr);
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
    return strcmp(sexp1->atom->word, sexp2->atom->word) == 0;
}

tok_t *sexp_atom(const sexp_t *s) {
    return (s && s->type == SEXP_ATOM) ? sexp->atom : NULL;
}

sexp_t *sexp_atom_set(sexp_t *s, tok_t *atom) {
    if (!s || !atom || s->type != ATOM) return NULL;
    s->atom = atom;
    return s;
}

#define SEXP_CXR(NAME, FIELD)                   \
    sexp_t *NAME(sexp_t *s) {                   \
        if (!s || s->type != SEXP_CONS)         \
            return NULL;                        \
        return s->cons->FIELD;                  \
    }                                           \

#define SEXP_CXR_SET(NAME, FIELD)               \
    sexp_t *NAME(sexp_t *s, sexp_t *e) {        \
        if (!s || !e || s->type != SEXP_CONS)   \
            return NULL;                        \
        s->cons->FIELD = e;                     \
        return s;                               \
    }                                           \

SEXP_CXR(sexp_car, car)
SEXP_CXR(sexp_cdr, cdr)
SEXP_CXR_SET(sexp_car_set, car)
SEXP_CXR_SET(sexp_cdr_set, cdr)

#undef SEXP_CXR
#undef SEXP_CXR_SET

sexp_t *sexp_cons(sexp_t *car, sexp_t *cdr) {
    return make_sexp_cons(car, cdr);
}

sexp_t *sexp_assq(sexp_t *alst, const sexp_t *symbol) {
    for (sexp_t *lst = alst; !sexp_is_null(lst); lst = lst->cdr) {
        sexp_t *pair = lst->car;
        if (sexp_is_eq(pair->car, symbol))
            return pair;
    }
    return sexp_null;
}

sexp_t *sexp_reverse(sexp_t *sexp) {
    sexp_t *prev = NULL;
    for (sexp_t *walk = sexp, *next = NULL; walk != NULL;) {
        next = walk->cdr;
        walk->cdr = prev;
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
        for (sexp_t *walk = sexp; !sexp_is_null(walk); walk = walk->cdr) {
            print_sexp(walk->car);
            if (!sexp_is_null(walk->cdr)) putchar(' ');
        }
        putchar(')');
    }
}
