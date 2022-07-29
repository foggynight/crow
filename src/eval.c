#include "eval.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "closure.h"
#include "env.h"
#include "error.h"
#include "sexp.h"
#include "token.h"
#include "types.h"

static sexp_t *crow_apply(sexp_t *proc, sexp_t *args);
static sexp_t *eval_list(sexp_t *list, sexp_t *env);
static sexp_t *special_form(sexp_t *form, sexp_t *env);

sexp_t *crow_eval(sexp_t *sexp, sexp_t *env) {
    switch (sexp->type) {
    case SEXP_NULL: error("crow_eval: null expression");
    case SEXP_SYMBOL: return env_lookup(env, sexp);
    case SEXP_NUM: return sexp;
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

static sexp_t *primitive_apply(sexp_t *proc, sexp_t *args) {
    return (*proc->func)(args);
}

static sexp_t *crow_apply(sexp_t *proc, sexp_t *args) {
    if (sexp_is_primitive(proc)) {
        return primitive_apply(proc, args);
    } else if (sexp_is_closure(proc)) {
        sexp_t *env = env_bind_formals(closure_env(proc),
                                       closure_args(proc), args);
        return crow_eval(closure_body(proc), env);
    } else { error("crow_apply: invalid procedure"); }
    return NULL;
}

static sexp_t *eval_list(sexp_t *list, sexp_t *env) {
    return sexp_is_null(list) ? list
        : sexp_cons(crow_eval(sexp_car(list), env),
                    eval_list(sexp_cdr(list), env));
}

// sexp -> (SEXP)
static sexp_t *sf_quote(sexp_t *sexp) {
    if (!sexp_is_null(sexp_cdr(sexp)))
        error("special_form: invalid quote form");
    return sexp_car(sexp);
}

// sexp -> (ARGS SEXP*)
// ARGS -> SYMBOL | (SEXP*) | (SEXP+ . SEXP)
static sexp_t *sf_lambda(sexp_t *sexp, sexp_t *env) {
    assert(sexp); assert(env);
    return sexp_closure(sexp, env);
}

// sexp -> (SEXP*)
static sexp_t *sf_begin(sexp_t *sexp, sexp_t *env) {
    assert(sexp); assert(env);
    sexp_t *res = sexp_null;
    for (sexp_t *walk = sexp; !sexp_is_null(walk); walk = sexp_cdr(walk))
        res = crow_eval(sexp_car(walk), env);
    return res;
}

// sexp   -> (CLAUSE*)
// CLAUSE -> (SEXP SEXP*)
static sexp_t *sf_cond(sexp_t *sexp, sexp_t *env) {
    for (sexp_t *walk = sexp; !sexp_is_null(walk); walk = sexp_cdr(walk)) {
        sexp_t *clause = sexp_car(walk);
        sexp_t *pred = sexp_car(clause);
        if (sexp_eq_p(pred, sexp_else) || sexp_true_p(crow_eval(pred, env)))
            return crow_eval(sexp_cons(sexp_begin, sexp_cdr(clause)), env);
    }
    return sexp_null;
}

// sexp -> (SYMBOL VALUE)
static sexp_t *sf_define(sexp_t *sexp, sexp_t *env) {
    sexp_t *key = sexp_car(sexp);
    sexp_t *datum = crow_eval(sexp_cadr(sexp), env);
    env_insert(env, sexp_cons(key, datum));
    return sexp_null;
}

static sexp_t *special_form(sexp_t *form, sexp_t *env) {
    assert(form); assert(env);

    const sexp_t *head = sexp_car(form);
    if (!sexp_is_symbol(head)) return NULL;

    char *name = tok_word(sexp_symbol(head));
    sexp_t *rest = sexp_cdr(form);

    if      (strcmp(name, "quote")  == 0) { return sf_quote(rest); }
    else if (strcmp(name, "lambda") == 0) { return sf_lambda(rest, env); }
    else if (strcmp(name, "begin")  == 0) { return sf_begin(rest, env); }
    else if (strcmp(name, "cond")   == 0) { return sf_cond(rest, env); }
    else if (strcmp(name, "define") == 0) { return sf_define(rest, env); }

    return NULL;
}
