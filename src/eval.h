// eval.h - S-expression evaluator

#ifndef EVAL_H
#define EVAL_H

#include "env.h"
#include "sexp.h"

sexp_t *crow_eval(sexp_t *sexp, env_t *env);

#endif // EVAL_H
