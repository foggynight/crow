#include "sexp.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "token.h"

#define tok_null  &(tok_t){ TOK_NULL,  "()" }
#define tok_quote &(tok_t){ TOK_QUOTE, "quote" }

sexp_t *sexp_null  = &(sexp_t){ SEXP_NULL, tok_null, NULL, NULL };
sexp_t *sexp_quote = &(sexp_t){ SEXP_ATOM, tok_quote, NULL, NULL };

sexp_t *make_sexp(sexp_type_t type, tok_t *atom, sexp_t *car, sexp_t *cdr) {
    sexp_t *ret = calloc(1, sizeof(sexp_t));
    if (!ret) error("make_sexp: failed to allocate sexp_t");

    switch (type) {
    case SEXP_NULL: break;
    case SEXP_ATOM: ret->atom = atom; break;
    case SEXP_CONS: ret->car = car; ret->cdr = cdr; break;
    default: error("make_sexp: invalid type argument: %d", type);
    }

    return ret;
}

sexp_t *make_sexp_null(void) {
    return make_sexp(SEXP_NULL, tok_quote, NULL, NULL);
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

bool sexp_is_atom(const sexp_t *sexp) {
    return sexp->type == SEXP_ATOM || sexp->type == SEXP_NULL;
}

bool sexp_is_cons(const sexp_t *sexp) {
    return sexp->type == SEXP_CONS;
}

sexp_t *sexp_cons(sexp_t *car, sexp_t *cdr) {
    return make_sexp_cons(car, cdr);
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
        for (sexp_t *walk = sexp; walk != NULL; walk = walk->cdr) {
            print_sexp(walk->car);
            if (walk->cdr) putchar(' ');
        }
        putchar(')');
    }
}
