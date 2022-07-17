#include "eval.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "env.h"
#include "error.h"
#include "sexp.h"
#include "token.h"

static sexp_t *crow_apply(sexp_t *proc, sexp_t *args);
static sexp_t *eval_list(sexp_t *list, env_t *env);
static sexp_t *special_form(sexp_t *form, env_t *env);

sexp_t *crow_eval(sexp_t *sexp, env_t *env) {
    switch (sexp->type) {
    case SEXP_NULL: error("crow_eval: null expression");
    case SEXP_SYMBOL: return env_lookup(env, sexp);
    case SEXP_NUMBER: return sexp;
    case SEXP_CONS: {
        sexp_t *result = special_form(sexp, env);
        return result ? result
            : crow_apply(crow_eval(sexp_car(sexp), env),
                         eval_list(sexp_cdr(sexp), env));
    }
    default: error("crow_eval: invalid expression type");
    }
    return NULL;
}

static sexp_t *crow_apply(sexp_t *proc, sexp_t *args) {
    return NULL;
}

static sexp_t *eval_list(sexp_t *list, env_t *env) {
    return sexp_cons(crow_eval(sexp_car(list), env),
                     eval_list(sexp_cdr(list), env));
}

static sexp_t *special_form(sexp_t *form, env_t *env) {
    assert(form); assert(env);

    const sexp_t *head = sexp_car(form);
    if (!sexp_is_symbol(head)) return NULL;

    const char *name = tok_word(sexp_symbol(head));
    const sexp_t *args = sexp_cdr(form);

    if        (strcmp(name, "quote") == 0) {
        return sexp_car(args);
    } else if (0 && strcmp(name, "...") == 0) {

    }

    return NULL;
}
