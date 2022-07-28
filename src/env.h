// env.h - environment

#ifndef ENV_H
#define ENV_H

#include "sexp.h"
#include "types.h"

sexp_t *make_env(void);     // Make a new environment with a single empty frame.

sexp_t *env_grow(sexp_t *env);                  // Add empty frame onto env.
sexp_t *env_expand(sexp_t *env, sexp_t *frame); // Add given frame onto of env.

// Insert (symbol . datum) pair into the top frame of env.
sexp_t *env_insert(sexp_t *env, sexp_t *pair);

// Fetch (symbol . datum) pair in env; searches from the top frame down, returns
// the first pair found whose car is symbol, or null if no pair is found.
sexp_t *env_fetch(const sexp_t *env, const sexp_t *symbol);

// Lookup symbol in env using env_fetch, returns the datum of the pair if one is
// found, or null if no pair was found.
sexp_t *env_lookup(const sexp_t *env, const sexp_t *symbol);

sexp_t *env_bind_formals(sexp_t *env, sexp_t *arg_symbols, sexp_t *arg_values);

#endif // ENV_H
