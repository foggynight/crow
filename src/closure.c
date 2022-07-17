#include "closure.h"

#include <assert.h>
#include <stdlib.h>

#include "env.h"
#include "sexp.h"
#include "types.h"

sexp_t *make_closure(sexp_t *args, sexp_t *body, sexp_t *env) {
    assert(args); assert(body); assert(env);
    return sexp_cons(args, sexp_cons(body, sexp_cons(env, sexp_null)));
}

sexp_t *closure_args(const sexp_t *s) { assert(s); return sexp_car(s); }
sexp_t *closure_body(const sexp_t *s) { assert(s); return sexp_cadr(s); }
sexp_t *closure_env(const sexp_t *s)  { assert(s); return sexp_caddr(s); }
