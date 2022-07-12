#include "sexp.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "error.h"

// cons ------------------------------------------------------------------------

cons_t *make_cons(sexp_t *car, sexp_t *cdr) {
    cons_t *cons = calloc(sizeof(cons_t));
    if (!cons) error("make_cons: failed to allocate cons_t");
    cons->car = car;
    cons->cdr = cdr;
    return cons;
}

void dest_cons(cons_t *cons) {
    dest_sexp(cons->car);
    dest_sexp(cons->cdr);
    free(cons);
}

sexp_t *cons_car(cons_t *cons) { return cons->car; }
sexp_t *cons_cdr(cons_t *cons) { return cons->cdr; }
cons_t *cons_cons(cons_t *car, cons_t *cdr) { return make_cons(car, cdr); }

// sexp ------------------------------------------------------------------------

sexp_t *make_sexp(tok_t *atom, vec_t *list) {
    assert(!(atom && list));

    sexp_t *ret = calloc(sizeof(sexp_t));
    if (!ret) error("make_sexp: failed to allocate sexp_t");

    if (atom) ret->atom = atom;
    else if (list) ret->list = list;

    return ret;
}

void dest_sexp(sexp_t *sexp) {
    assert(sexp);
    if (sexp_is_list(sexp))
        dest_vec(ast->list);
    free(sexp);
}

bool sexp_is_atom(const sexp_t *sexp) {
    assert(sexp);
    return sexp->atom != NULL;
}

bool sexp_is_list(const sexp_t *sexp) {
    assert(sexp);
    return sexp->list != NULL;
}
