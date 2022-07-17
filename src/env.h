// env.h - environment

#ifndef ENV_H
#define ENV_H

#include "sexp.h"
#include "types.h"

sexp_t *make_env(void);     // Make a new environment with a single empty frame.

// Add an empty frame on top of env.
sexp_t *env_expand(sexp_t *env);

// Insert (symbol . datum) pair into the top frame of env.
sexp_t *env_insert(sexp_t *env, sexp_t *pair);

// Fetch (symbol . datum) pair in env; searches from the top frame down, returns
// the first pair found whose car is symbol, or null if no pair is found.
sexp_t *env_fetch(const sexp_t *env, const sexp_t *symbol);

// Lookup symbol in env using env_fetch, returns the datum of the pair if one is
// found, or null if no pair was found.
sexp_t *env_lookup(const sexp_t *env, const sexp_t *symbol);

#endif // ENV_H
