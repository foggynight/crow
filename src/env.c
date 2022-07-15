#include "env.h"

#include <assert.h>
#include <stdlib.h>

#include "sexp.h"

env_t *make_env(void) {
    env_t *env = malloc(sizeof env);
    if (!env) error("make_env: failed to allocate env");
    env->frames = sexp_cons(sexp_null, sexp_null);
    return env;
}

void dest_env(env_t *env) {}

env_t *env_expand(env_t *env) {
    assert(env);
    env->frames = sexp_cons(sexp_null, env->frames);
    return env;
}

env_t *env_insert(env_t *env, sexp_t *pair) {
    assert(env); assert(!sexp_is_null(env->frames));
    env->frames->car = sexp_cons(pair, env->frames->car);
    return env;
}

sexp_t *env_fetch(env_t *env, sexp_t *symbol) {
    for (sexp_t *lst = env->frames; !sexp_is_null(lst); lst = lst->cdr) {
        sexp_t *pair = sexp_assq(env->frames->car, symbol);
        if (!sexp_is_null(pair)) return pair;
    }
    return sexp_null;
}

sexp_t *env_lookup(env_t *env, sexp_t *symbol) {
    const sexp_t *pair = env_fetch(env, symbol);
    return sexp_is_null(pair) ? sexp_null : pair->cdr;
}
