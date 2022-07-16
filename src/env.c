#include "env.h"

#include <assert.h>
#include <stdlib.h>

#include "error.h"
#include "sexp.h"

env_t *make_env(void) {
    env_t *env = malloc(sizeof env);
    if (!env) error("make_env: failed to allocate env");
    env->frames = sexp_cons(sexp_null, sexp_null);
    return env;
}

env_t *env_expand(env_t *env) {
    assert(env);
    env->frames = sexp_cons(sexp_null, env->frames);
    return env;
}

env_t *env_insert(env_t *env, sexp_t *pair) {
    assert(env); assert(!sexp_is_null(env->frames));
    sexp_car_set(env->frames, sexp_cons(pair, sexp_car(env->frames)));
    return env;
}

sexp_t *env_fetch(const env_t *env, const sexp_t *symbol) {
    for (sexp_t *lst = env->frames; !sexp_is_null(lst); lst = sexp_cdr(lst)) {
        sexp_t *pair = sexp_assq(sexp_car(env->frames), symbol);
        if (!sexp_is_null(pair)) return pair;
    }
    return sexp_null;
}

sexp_t *env_lookup(const env_t *env, const sexp_t *symbol) {
    sexp_t *pair = env_fetch(env, symbol);
    return sexp_is_null(pair) ? sexp_null : sexp_cdr(pair);
}
