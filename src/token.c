#include "token.h"

#include <stdlib.h>

#include "error.h"

tok_t *make_tok(tok_type_t type, char *word) {
    tok_t *tok = malloc(sizeof *tok);
    if (!tok)
        error("make_tok: malloc failed");

    tok->type = type;
    tok->word = word;

    return tok;
}

void dest_tok(tok_t *tok) {
    free(tok->word);
    free(tok);
}
