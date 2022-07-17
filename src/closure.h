#ifndef CLOSURE_H
#define CLOSURE_H

#include "env.h"
#include "sexp.h"
#include "types.h"

sexp_t *make_closure(sexp_t *args, sexp_t *body, sexp_t *env);

sexp_t *closure_args(const sexp_t *clos);
sexp_t *closure_body(const sexp_t *clos);
sexp_t *closure_env(const sexp_t *clos);

#endif // CLOSURE_H
