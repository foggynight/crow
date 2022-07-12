#include "token.h"

#include <stdbool.h>
#include <stdlib.h>

#include "error.h"

tok_t *make_tok(tok_type_t type, char *word) {
    tok_t *tok = malloc(sizeof *tok);
    if (!tok) error("make_tok: malloc failed");
    tok->type = type;
    tok->word = word;
    return tok;
}

void dest_tok(tok_t *tok) {
    free(tok->word);
    free(tok);
}

bool tok_type_is_atom(tok_type_t type) {
    return type == TOK_SYMBOL
        || type == TOK_NUMBER
        || type == TOK_BOOL
        || type == TOK_CHARACTER
        || type == TOK_STRING;
}

bool tok_is_atom(tok_t *tok) {
    return tok ? tok_type_is_atom(tok->type) : false;
}
