// TODO: Use macros to do this better.

#include "primitive.h"

#include "sexp.h"
#include "types.h"

sexp_t *prim_cons(sexp_t *args) { return sexp_cons(sexp_car(args),
                                                   sexp_cadr(args)); }
static sexp_t *prim_cons_pair(void) {
    return sexp_cons(make_sexp_symbol(make_tok(TOK_SYMBOL, "cons")),
                     sexp_primitive(prim_cons));
}

sexp_t *prim_car(sexp_t *args) { return sexp_car(sexp_car(args)); }
static sexp_t *prim_car_pair(void) {
    return sexp_cons(make_sexp_symbol(make_tok(TOK_SYMBOL, "car")),
                     sexp_primitive(prim_car));
}

sexp_t *prim_cdr(sexp_t *args) { return sexp_cdr(sexp_car(args)); }
static sexp_t *prim_cdr_pair(void) {
    return sexp_cons(make_sexp_symbol(make_tok(TOK_SYMBOL, "cdr")),
                     sexp_primitive(prim_cdr));
}

sexp_t *prim_frame(void) {
    return sexp_cons(prim_cons_pair(),
                     sexp_cons(prim_car_pair(),
                               sexp_cons(prim_cdr_pair(), sexp_null)));
}
