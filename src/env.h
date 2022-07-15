// env.h - environment
//
// Environments are represented by a structure containing a list of frames,
// where each frame is an association list of (symbol . datum) pairs.

#ifndef ENV_H
#define ENV_H

#include "sexp.h"

typedef struct env {
    sexp_t *frames;
} env_t;

env_t *make_env(void);     // Make a new environment with a single empty frame.
void dest_env(env_t *env); // Destroy env and the frames it contains.

// Add an empty frame on top of env.
env_t *env_expand(env_t *env);

// Insert (symbol . datum) pair into the top frame of env.
env_t *env_insert(env_t *env, sexp_t *pair);

// Fetch (symbol . datum) pair in env; searches from the top frame down, returns
// the first pair found whose car is symbol, or null if no pair is found.
sexp_t *env_fetch(const env_t *env, const sexp_t *symbol);

// Lookup symbol in env using env_fetch, returns the datum of the pair if one is
// found, or null if no pair was found.
sexp_t *env_lookup(const env_t *env, const sexp_t *symbol);

#endif // ENV_H
