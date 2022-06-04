#include "token.h"
#include "vector.h"

tok_t *lex_word(char *word); // Tokenize word and return the token.

// Tokenize str and push tokens onto toks. This function allocates a string for
// each token which must be freed.
void lex_str(char *str, vec_t *toks);
