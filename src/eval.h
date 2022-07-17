// eval.h - S-expression evaluator

#ifndef EVAL_H
#define EVAL_H

#include "sexp.h"
#include "types.h"

sexp_t *crow_eval(sexp_t *sexp, sexp_t *env);

#endif // EVAL_H
