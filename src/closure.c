#include "closure.h"

#include <assert.h>
#include <stdlib.h>

#include "env.h"
#include "sexp.h"
#include "types.h"

sexp_t *make_closure(sexp_t *args, sexp_t *body, sexp_t *env) {
    assert(args); assert(sexp_is_cons(args));
    assert(body); assert(sexp_is_cons(body));
    assert(env); assert(sexp_is_cons(env));
    return sexp_cons();
}

static sexp_t *ref(const sexp_t *clos,
                   sexp_t *(*func)(const sexp_t *s))
{
    assert(clos);
    assert(func);
    return (sexp_type(clos) == SEXP_CLOSURE) ? func(clos) : NULL;
}

sexp_t *closure_args(const sexp_t *s) { assert(s); return ref(s, sexp_car); }
sexp_t *closure_body(const sexp_t *s) { assert(s); return ref(s, sexp_cadr); }
sexp_t *closure_env(const sexp_t *s)  { assert(s); return ref(s, sexp_caddr); }
