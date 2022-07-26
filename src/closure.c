#include "closure.h"

#include <assert.h>
#include <stdlib.h>

#include "env.h"
#include "sexp.h"
#include "types.h"

sexp_t *make_closure(sexp_t *args, sexp_t *body, sexp_t *env) {
    assert(args); assert(body); assert(env);
    assert(sexp_is_list(args)); assert(sexp_is_list(env));
    sexp_t *clos = sexp_cons(args, sexp_cons(body, sexp_cons(env, sexp_null)));
    sexp_type_set(clos, SEXP_CLOSURE);
    return clos;
}

static sexp_t *ref(const sexp_t *clos,
                   sexp_t *(*func)(const sexp_t *s))
{
    assert(clos); assert(func);
    return (sexp_is_closure(clos)) ? func(clos) : NULL;
}

sexp_t *closure_args(const sexp_t *s) { assert(s); return ref(s, sexp_car); }
sexp_t *closure_body(const sexp_t *s) { assert(s); return ref(s, sexp_cadr); }
sexp_t *closure_env(const sexp_t *s)  { assert(s); return ref(s, sexp_caddr); }
