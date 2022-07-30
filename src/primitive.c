// All primitive functions take a single sexp argument and return a sexp. The
// arguments of the primitive itself are contained in the sexp argument, which
// is always a list.

// TODO: Check types in all primitives.

#include "primitive.h"

#include "error.h"
#include "sexp.h"
#include "types.h"

static sexp_t *null(sexp_t *a) { return bool2sexp(sexp_is_null(sexp_car(a))); }

static sexp_t *cons(sexp_t *a) { return sexp_cons(sexp_car(a), sexp_cadr(a)); }
static sexp_t *car(sexp_t *args) { return sexp_car(sexp_car(args)); }
static sexp_t *cdr(sexp_t *args) { return sexp_cdr(sexp_car(args)); }

#define NUM_PRED(NAME, PRED)                                    \
    static sexp_t *num_ ## NAME(sexp_t *args) {                 \
        bool res = true;                                        \
        bool first = true;                                      \
        num_integer_t last;                                     \
        for (sexp_t *walk = args;                               \
             !sexp_is_null(walk);                               \
             walk = sexp_cdr(walk)) {                           \
            sexp_t *num = sexp_car(walk);                       \
            if (!sexp_is_num(num))                              \
                error("bad argument type: not a number");       \
            if (first) {                                        \
                first = false;                                  \
                last = sexp_num(num)->integer;                  \
            } else {                                            \
                num_integer_t curr = sexp_num(num)->integer;    \
                if (last PRED curr) {                           \
                    res = false;                                \
                    break;                                      \
                }                                               \
                last = curr;                                    \
            }                                                   \
        }                                                       \
        return bool2sexp(res);                                  \
    }                                                           \

NUM_PRED(eq, !=);
NUM_PRED(neq, ==);
NUM_PRED(lt, >=);
NUM_PRED(gt, <=);
NUM_PRED(lte, >);
NUM_PRED(gte, <);

#undef NUM_PRED

static sexp_t *add(sexp_t *args) {
    num_integer_t accum = 0;
    for (sexp_t *walk = args; !sexp_is_null(walk); walk = sexp_cdr(walk)) {
        sexp_t *num = sexp_car(walk);
        if (!sexp_is_num(num))
            error("prim_plus: bad argument type: not a number");
        accum += sexp_num(num)->integer;
    }
    return make_sexp_num(make_num_integer(accum));
}

static sexp_t *sub(sexp_t *args) {
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

//------------------------------------------------------------------------------

#define PRIM_PAIR(PRIM_NAME, FUNC_NAME)                                     \
    static sexp_t *FUNC_NAME ## _pair(void) {                               \
        return sexp_cons(make_sexp_symbol(make_tok(TOK_SYMBOL, PRIM_NAME)), \
                         sexp_primitive(FUNC_NAME));                        \
    }                                                                       \

PRIM_PAIR("null?", null);
PRIM_PAIR("cons", cons);
PRIM_PAIR("car", car);
PRIM_PAIR("cdr", cdr);
PRIM_PAIR("=", num_eq);
PRIM_PAIR("<>", num_neq);
PRIM_PAIR("<", num_lt);
PRIM_PAIR(">", num_gt);
PRIM_PAIR("<=", num_lte);
PRIM_PAIR(">=", num_gte);
PRIM_PAIR("+", add);
PRIM_PAIR("-", sub);

#undef PRIM_PAIR

sexp_t *prim_frame(void) {
    return
        sexp_cons(null_pair(),
        sexp_cons(cons_pair(),
        sexp_cons(car_pair(),
        sexp_cons(cdr_pair(),
        sexp_cons(num_eq_pair(),
        sexp_cons(num_neq_pair(),
        sexp_cons(num_lt_pair(),
        sexp_cons(num_gt_pair(),
        sexp_cons(num_lte_pair(),
        sexp_cons(num_gte_pair(),
        sexp_cons(add_pair(),
        sexp_cons(sub_pair(),
                  sexp_null
    ))))))))))));
}
