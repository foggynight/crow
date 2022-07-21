#include "env.h"

#include <assert.h>

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
