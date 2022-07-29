// All primitive functions take a single sexp argument and return a sexp. The
// arguments of the primitive itself are contained in the sexp argument, which
// is always a list.

// TODO: Use macros to do this better.

#include "primitive.h"

#include "error.h"
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

sexp_t *prim_add(sexp_t *args) {
    num_integer_t accum = 0;
    for (sexp_t *walk = args; !sexp_is_null(walk); walk = sexp_cdr(walk)) {
        sexp_t *num = sexp_car(walk);
        if (!sexp_is_num(num))
            error("prim_plus: bad argument type: not a number");
        accum += sexp_num(num)->integer;
    }
    return make_sexp_num(make_num_integer(accum));
}
static sexp_t *prim_add_pair(void) {
    return sexp_cons(make_sexp_symbol(make_tok(TOK_SYMBOL, "+")),
                     sexp_primitive(prim_add));
}

sexp_t *prim_num_lt(sexp_t *args) {
    bool res = true;
    bool first = true;
    num_integer_t last;
    for (sexp_t *walk = args; !sexp_is_null(walk); walk = sexp_cdr(walk)) {
        sexp_t *num = sexp_car(walk);
        if (!sexp_is_num(num))
            error("prim_num_lt: bad argument type: not a number");
        if (first) {
            first = false;
            last = sexp_num(num)->integer;
        } else {
            num_integer_t curr = sexp_num(num)->integer;
            if (last >= curr) {
                res = false;
                break;
            }
            last = curr;
        }
    }
    return res ? sexp_t_sym : sexp_null;
}
static sexp_t *prim_num_lt_pair(void) {
    return sexp_cons(make_sexp_symbol(make_tok(TOK_SYMBOL, "<")),
                     sexp_primitive(prim_num_lt));
}

sexp_t *prim_sub(sexp_t *args) {
    if (sexp_is_null(args))
        error("prim_sub: too few arguments");
    sexp_t *num = sexp_car(args);
    if (!sexp_is_num(num))
        error("prim_plus: bad argument type: not a number");
    num_integer_t value = sexp_num(num)->integer;
    if (sexp_is_null(sexp_cdr(args))) {
        value = -value;
    } else {
        for (sexp_t *walk = sexp_cdr(args);
             !sexp_is_null(walk);
             walk = sexp_cdr(walk))
        {
            sexp_t *num = sexp_car(walk);
            if (!sexp_is_num(num))
                error("prim_plus: bad argument type: not a number");
            value -= sexp_num(num)->integer;
        }
    }
    return make_sexp_num(make_num_integer(value));
}
static sexp_t *prim_sub_pair(void) {
    return sexp_cons(make_sexp_symbol(make_tok(TOK_SYMBOL, "-")),
                     sexp_primitive(prim_sub));
}

//------------------------------------------------------------------------------

sexp_t *prim_frame(void) {
    return
        sexp_cons(prim_cons_pair(),
        sexp_cons(prim_car_pair(),
        sexp_cons(prim_cdr_pair(),
        sexp_cons(prim_num_lt_pair(),
        sexp_cons(prim_add_pair(),
        sexp_cons(prim_sub_pair(),
                  sexp_null
            ))))));
}
