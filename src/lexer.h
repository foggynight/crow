#include <stdio.h>

#include "token.h"
#include "vector.h"

tok_t *lex_word(char *word); // Tokenize word and return the token.
void lex_str(char *str, vec_t *toks); // Tokenize str and push tokens onto toks.
