#include "sexp.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "token.h"

// cons ------------------------------------------------------------------------

cons_t *make_cons(sexp_t *car, cons_t *cdr) {
    cons_t *cons = calloc(1, sizeof(cons_t));
    if (!cons) error("make_cons: failed to allocate cons_t");
    cons->car = car;
    cons->cdr = cdr;
    return cons;
}

void dest_cons(cons_t *cons) {
    dest_sexp(cons->car);
    dest_cons(cons->cdr);
    free(cons);
}

sexp_t *cons_car(cons_t *cons) { return cons->car; }
cons_t *cons_cdr(cons_t *cons) { return cons->cdr; }
cons_t *cons_cons(sexp_t *car, cons_t *cdr) { return make_cons(car, cdr); }

// sexp ------------------------------------------------------------------------

sexp_t *make_sexp(tok_t *atom, cons_t *list) {
    assert(!(atom && list));

    sexp_t *ret = calloc(1, sizeof(sexp_t));
    if (!ret) error("make_sexp: failed to allocate sexp_t");

    if (atom) ret->atom = atom;
    else if (list) ret->list = list;

    return ret;
}

void dest_sexp(sexp_t *sexp) {
    assert(sexp);
    if (sexp_is_list(sexp))
        dest_cons(sexp->list);
    free(sexp);
}

bool sexp_is_atom(const sexp_t *sexp) {
    assert(sexp);
    return sexp->atom != NULL;
}

bool sexp_is_list(const sexp_t *sexp) {
    assert(sexp);
    return sexp->atom == NULL;
}

sexp_t *sexp_cons(sexp_t *car, sexp_t *cdr) {
    cdr->list = cons_cons(car, cdr->list);
    return cdr;
}

sexp_t *sexp_reverse(sexp_t *sexp) {
    cons_t *prev = NULL;
    for (cons_t *walk = sexp->list, *next = NULL;
         walk != NULL;
         walk = next)
    {
        next = walk->cdr;
        walk->cdr = prev;
        prev = walk;
    }
    sexp->list = prev;
    return sexp;
}

void print_sexp(sexp_t *sexp) {
    if (sexp_is_atom(sexp)) {
        printf("%s", sexp->atom->word);
    } else {
        putchar('(');
        for (cons_t *walk = sexp->list; walk != NULL;) {
            print_sexp(cons_car(walk));
            cons_t *next = cons_cdr(walk);
            if (next) putchar(' ');
            walk = next;
        }
        putchar(')');
    }
}
