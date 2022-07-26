#include "env.h"

#include <assert.h>

#include "error.h"
#include "sexp.h"
#include "types.h"

sexp_t *make_env(void) {
    return sexp_cons(sexp_null, sexp_null);
}

sexp_t *env_expand(sexp_t *env) {
    assert(env);
    return sexp_cons(sexp_null, env);
}

sexp_t *env_insert(sexp_t *env, sexp_t *pair) {
    assert(env); assert(!sexp_is_null(env));
    sexp_car_set(env, sexp_cons(pair, sexp_car(env)));
    return env;
}

sexp_t *env_fetch(const sexp_t *env, const sexp_t *symbol) {
    for (const sexp_t *lst = env; !sexp_is_null(lst); lst = sexp_cdr(lst)) {
        sexp_t *pair = sexp_assq(sexp_car(lst), symbol);
        if (!sexp_is_null(pair)) return pair;
    }
    return sexp_null;
}

sexp_t *env_lookup(const sexp_t *env, const sexp_t *symbol) {
    sexp_t *pair = env_fetch(env, symbol);
    return sexp_is_null(pair) ? sexp_null : sexp_cdr(pair);
}

sexp_t *env_bind_formals(sexp_t *env,         // env to add a frame onto
                         sexp_t *arg_symbols, // list of arg symbols
                         sexp_t *arg_values)  // list of arg values
{
    sexp_t *frame = sexp_null;
    sexp_t *sym, *val;
    for (sym = arg_symbols, val = arg_values;
         !sexp_is_null(sym) && !sexp_is_null(val);
         sym = sexp_cdr(sym), val = sexp_cdr(val))
    {
        sexp_t *cons = sexp_cons(sexp_car(sym), sexp_car(val));
        frame = sexp_cons(cons, frame);
    }
    if (!sexp_is_null(sym) || !sexp_is_null(val))
        error("env_bind_formals: different number of arguments and values");
    return sexp_cons(frame, env);
}
